#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A Jump and Branch Finder Utility for decoding ARM and Thumb instructions in binary files.

Python: 3.10+
License: MIT
Authors: EXL, fkcoder, usernameak, MotoFan.Ru, GitHub Copilot (ChatGPT 4o)
Date: 11-Feb-2025
Version: 1.0
Note: Install capstone first: `pip install capstone`
"""

import capstone

import argparse


def is_valid_armv4t_instruction(inst, opts):
	return inst.mnemonic not in {'blx', 'bxj'} if opts['armv4t'] else True


def nake_address(op_str, opts):
	op_str = op_str.strip()
	if op_str.startswith('#'):
		try:
			address = int(op_str[1:], 16)
			return (address in opts['filter'], f'#0x{address:08X}')
		except ValueError:
			pass
	return (True, op_str)


def process_i(inst_buff, offset, mode, opts) -> None:
	md = opts['mda'] if mode == 'A' else opts['mdt']
	for inst in md.disasm(inst_buff, offset):
		if opts['group'] in inst.groups:
			bytecode = ' '.join([f'{b:02X}' for b in inst.bytes])
			op_addr, op_str = nake_address(inst.op_str, opts)
			if op_addr and is_valid_armv4t_instruction(inst, opts):
				print(f'{mode} : 0x{inst.address:08X} : {bytecode:<11} : {inst.mnemonic.upper():<6} {op_str}')


def read_binary_file(opts):
	with open(opts['file'], 'rb') as file:
		file_offset = 0
		while True:
			inst_buff = file.read(0x04)
			if not inst_buff:
				break
			process_i(inst_buff, opts['offset'] + file_offset, 'T', opts)
			process_i(inst_buff, opts['offset'] + file_offset, 'A', opts)
			file_offset += len(inst_buff)


def set_endian(big_endian):
	return capstone.CS_MODE_BIG_ENDIAN if big_endian else capstone.CS_MODE_LITTLE_ENDIAN


def set_group(group):
	grp = group.lower()
	if grp == 'jump':
		return capstone.CS_GRP_JUMP
	elif grp == 'branch':
		return capstone.CS_GRP_BRANCH_RELATIVE
	elif grp == 'call':
		return capstone.CS_GRP_CALL
	return capstone.CS_GRP_JUMP


def parse_range(range_str):
	range_start, range_end = range_str.split('-')
	return range(int(range_start, 16), int(range_end, 16))


def parse_hex(value):
	try:
		return int(value, 16)
	except ValueError:
		raise argparse.ArgumentTypeError(f'Invalid hexadecimal value: "{value}"')


def main():
	hlp = {
		'D': 'A Jump and Branch Finder Utility for decoding ARM and Thumb instructions in binary files, 11-Feb-2025',
		'f': 'path to the binary file (BIN or SMG)',
		'x': 'global firmware offset, default "0x10000000"',
		'r': 'address range filter, default "0x00000000-0xFFFFFFFF"',
		'b': 'CPU endianness: Little-Endian or Big-Endian, default LE',
		'a': 'use ARMv4T instruction set (no BLX, no BXJ), default False',
		'g': 'instruction group (Jump, Branch, Call), default Jump',
	}
	epl = """examples:
	# Little-Endian, Big-Endian, ARMv4T, and start offset:
	python jumper_arm.py -f BOOT_0826.bin
	python jumper_arm.py -f BOOT_0826.bin -b
	python jumper_arm.py -f BOOT_0826.bin -b -x 0xA0000000
	python jumper_arm.py -f BOOT_0826.bin -b -a

	# Groups:
	python jumper_arm.py -f BOOT_0826.bin -b -g Jump
	python jumper_arm.py -f BOOT_0826.bin -b -g Branch
	python jumper_arm.py -f BOOT_0826.bin -b -g Call

	# Filters:
	python jumper_arm.py -f BOOT_0826.bin -b -r 0x10000000-0x10001000
	python jumper_arm.py -f BOOT_0826.bin -b -a -g Call -r 0x10080000-0x11500000
	"""
	pa = argparse.ArgumentParser(description=hlp['D'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	pa.add_argument('-f', '--file', required=True, metavar='FILE', help=hlp['f'])
	pa.add_argument('-x', '--offset', type=parse_hex, default=0x10000000, metavar='OFFSET', help=hlp['x'])
	pa.add_argument('-r', '--filter', type=parse_range, default='0x00000000-0xFFFFFFFF', metavar='RANGE', help=hlp['r'])
	pa.add_argument('-b', '--big-endian', action='store_true', help=hlp['b'])
	pa.add_argument('-a', '--armv4t', action='store_true', help=hlp['a'])
	pa.add_argument('-g', '--group', type=str, default='All', metavar='GROUP', help=hlp['g'])

	args = pa.parse_args()

	opts = {
		'file'       : args.file,
		'offset'     : args.offset,
		'filter'     : args.filter,
		'big-endian' : args.big_endian,
		'armv4t'     : args.armv4t,
		'group'      : set_group(args.group),
		'mda'        : capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_ARM   + set_endian(args.big_endian)),
		'mdt'        : capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_THUMB + set_endian(args.big_endian)),
	}

	opts['mda'].detail = True
	opts['mdt'].detail = True

	read_binary_file(opts)


if __name__ == '__main__':
	main()
