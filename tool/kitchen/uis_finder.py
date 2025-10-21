#!/usr/bin/env python3

"""
A utility for analyzing Motorola ARM firmware binaries to identify UIS_ functions.
Searches for strings matching '\nUIS_* enter', locates corresponding ADR instructions,
and finds function entry points marked by PUSH instructions in both ARM and Thumb modes.
Outputs results in the format '0xADDRESS T/A UIS_FUNCTION' to terminal or file,
with statistics on found strings and functions printed to terminal.

Python: 3.10+
License: MIT
Authors: fkcoder, Grok 3, xAI
Date: 26-May-2025
Version: 1.0
"""

import re
import struct
import argparse


def decode_arm_immediate(instruction):
	"""Decode immediate value for ARM instructions (8-bit with rotation)."""
	rotate = (instruction >> 8) & 0xF
	imm8 = instruction & 0xFF
	if rotate == 0:
		return imm8
	else:
		return (imm8 >> (rotate * 2)) | ((imm8 << (32 - rotate * 2)) & 0xFFFFFFFF)


def decode_instruction(data, offset, arm_mode):
	"""Decode instruction at given offset for ARM or Thumb mode."""
	if arm_mode:
		if len(data) < offset + 4:
			return None
		return struct.unpack('>I', data[offset:offset + 4])[0]  # 32-bit ARM instruction
	else:
		if len(data) < offset + 2:
			return None
		return struct.unpack('>H', data[offset:offset + 2])[0]  # 16-bit Thumb instruction


def is_adr_instruction(instruction, arm_mode):
	"""Check if instruction is ADR (or ADD/SUB with PC for ARM)."""
	if arm_mode:
		cond = (instruction >> 28) & 0xF
		if cond != 0xE:  # Only unconditional instructions
			return False
		opcode = (instruction >> 21) & 0xF
		rn = (instruction >> 16) & 0xF
		if rn == 0xF and (opcode == 0x4 or opcode == 0x2):  # ADD or SUB with PC
			return True
		return False
	else:
		return (instruction >> 12) == 0xA  # Thumb ADR


def is_push_instruction(instruction, arm_mode):
	"""Check if instruction is PUSH."""
	if arm_mode:
		return (instruction >> 16) == 0xE92D  # ARM PUSH: STMDB SP!, {...}
	else:
		return (instruction >> 8) == 0xB4 or (instruction >> 8) == 0xB5  # Thumb PUSH


def find_function_start(data, adr_addr, max_search=1024, arm_mode=False):
	"""Search for function start (PUSH instruction) backwards from ADR address."""
	step = 4 if arm_mode else 2
	current_addr = adr_addr - step
	while current_addr >= 0 and (adr_addr - current_addr) <= max_search:
		instruction = decode_instruction(data, current_addr, arm_mode)
		if instruction is None:
			break
		if is_push_instruction(instruction, arm_mode):
			function_start = current_addr
			while True:
				prev_addr = current_addr - step
				if prev_addr < 0 or (adr_addr - prev_addr) > max_search:
					break
				prev_instruction = decode_instruction(data, prev_addr, arm_mode)
				if prev_instruction is None or not is_push_instruction(prev_instruction, arm_mode):
					break
				current_addr = prev_addr
				function_start = current_addr
			return function_start, arm_mode
		current_addr -= step
	return None, arm_mode


def main(firmware_path, base_offset, output_file=None):
	"""Main function to search for UIS_ functions in ARM firmware."""
	try:
		with open(firmware_path, 'rb') as f:
			data = f.read()
	except FileNotFoundError:
		print(f'Error: File \'{firmware_path}\' not found.')
		return

	string_pattern = rb'\nUIS_[A-Za-z0-9_]+ enter'
	matches = list(re.finditer(string_pattern, data))

	if not matches:
		print('No strings matching \'\\nUIS_* enter\' found.')
		return

	total_strings = len(matches)
	functions_found_arm = 0
	functions_found_thumb = 0

	if output_file:
		try:
			out = open(output_file, 'w')
		except Exception as e:
			print(f'Error opening output file \'{output_file}\': {e}')
			return
	else:
		out = None

	for match in matches:
		string = match.group().decode('ascii')
		string_addr = match.start()
		uis_name = string[5:-6]  # Extract UIS_ function name

		if out is None:
			print(f'\nProcessing string: {string}')
		adr_found = False
		push_addr = None
		is_arm_mode = None
		search_range = 4096

		# Check both ARM and Thumb modes
		for arm_mode in [True, False]:
			current_addr = max(0, string_addr - search_range)
			step = 4 if arm_mode else 2
			while current_addr < string_addr:
				instruction = decode_instruction(data, current_addr, arm_mode)
				if instruction is None:
					current_addr += step
					continue
				if is_adr_instruction(instruction, arm_mode):
					pc = current_addr + (8 if arm_mode else 4)
					if arm_mode:
						opcode = (instruction >> 21) & 0xF
						imm32 = decode_arm_immediate(instruction)
						if opcode == 0x4:  # ADD
							target_addr = pc + imm32
						elif opcode == 0x2:  # SUB
							target_addr = pc - imm32
						else:
							current_addr += step
							continue
					else:
						offset = (instruction & 0xFF) << 2
						target_addr = (pc & 0xFFFFFFFC) + offset
					if abs(target_addr - string_addr) < 4:
						adr_found = True
						push_addr, is_arm_mode = find_function_start(data, current_addr, max_search=1024,
						                                             arm_mode=arm_mode)
						if push_addr is not None:
							if is_arm_mode:
								functions_found_arm += 1
							else:
								functions_found_thumb += 1
						break
				current_addr += step
			if adr_found:
				break  # Stop if ADR found in one mode

		if out is None:
			print(f'Found function UIS_{uis_name}')
			print(f'ADR instruction: {"Found" if adr_found else "Not found"}')
			print(f'PUSH instruction: {"Found" if push_addr is not None else "Not found"}')
		if push_addr is not None:
			mode_char = 'A' if is_arm_mode else 'T'
			adjusted_addr = base_offset + push_addr
			result = f'0x{adjusted_addr:X} {mode_char} UIS_{uis_name}'
			if out:
				out.write(result + '\n')
			else:
				print(result)

	if out:
		out.close()

	# Statistics always printed to terminal
	print(
		f'Statistics:\n- Strings found: {total_strings}\n- Functions found in ARM: {functions_found_arm}\n- Functions found in Thumb: {functions_found_thumb}')


if __name__ == '__main__':
	parser = argparse.ArgumentParser(
		description='Search for UIS_ functions in an ARM firmware binary by finding strings like "\\nUIS_* enter", their ADR instructions, and function entry points (PUSH).'
	)
	parser.add_argument(
		'firmware_path',
		help='Path to the ARM firmware binary file (e.g., L9_R452J_G_08.22.07R.smg)'
	)
	parser.add_argument(
		'base_offset',
		type=lambda x: int(x, 0),
		help='Base offset for address calculation (hex or decimal, e.g., 0x10092000)'
	)
	parser.add_argument(
		'-o', '--output',
		help='Optional path to output SYM file where results (e.g., "0xADDRESS T/A UIS_FUNCTION") will be written; if not specified, results are printed to terminal'
	)
	args = parser.parse_args()
	main(args.firmware_path, args.base_offset, args.output)
