#!/usr/bin/env python

"""
portkit.py script by EXL, 27-Nov-2023

ElfPack v1.0 PortKit Tool.
"""

import os
import sys
import subprocess

REGISTER_FUNCTION_INJECTION = 'APP_SyncML_MainRegister'
DO_NOT_CLEAN_THESE_FILES = [ 'lte2_irom.sym' ]

P2K_ELF_SDK_PATH = os.path.join('..', '..')
PAT_UTILITY_WINDOWS = os.path.join(P2K_ELF_SDK_PATH, 'tool', 'pat.exe')
PAT_UTILITY_LINUX = os.path.join(P2K_ELF_SDK_PATH, 'tool', 'pat')
PAT_UTILITY = PAT_UTILITY_WINDOWS if sys.platform.startswith('win') else PAT_UTILITY_LINUX


def clean_files(directory, files):
	for file in files:
		if file not in DO_NOT_CLEAN_THESE_FILES:
			os.remove(os.path.join(directory, file))


def generate_register_symbol_file(cg1_path, func, sym, name):
	create_register_pattern(func, sym, name + '.pat')
	pat_path = os.path.join('libgen', name + '.pat')
	find_functions_from_patterns(cg1_path, pat_path, '-no-ram-trans', '0x00000000', name + '.sym')
	clean_files('libgen', [name + '.pat'])


def create_register_pattern(func, sym, name):
	address = find_register_function_address(func, sym)[2:]
	with open(os.path.join('libgen', name), 'w') as output:
		print(f'Will write "Register D {address}" to "{name}" file!')
		output.write(f'Register D {address}\r\n')


def find_register_function_address(func, sym):
	with open(os.path.join('libgen', sym), 'r') as input:
		for line in input.read().splitlines():
			line = line.strip()
			if len(line) != 0 and not line.startswith('#'):
				address, mode, name = line.split(' ')
				if name == func:
					address_int = int(address, 16) + 1
					return f'0x{address_int:08X}'


def validate_sym_file(sym):
	syms = { }
	missed = [ ]
	with open(os.path.join('libgen', sym), 'r') as input:
		for line in input.read().splitlines():
			line = line.strip()
			if len(line) != 0 and not line.startswith('#'):
				address, mode, name = line.split(' ')
				if not syms.get(name, None):
					syms[name] = address
				else:
					first_address = syms[name]
					print(f'Warning! Dublicates:\r\n\r\n\t{first_address}:{name}\r\n\t{address}:{name}')
					return False
			elif line.startswith('# NOT_FOUND: '):
				mode, name = line.replace('# NOT_FOUND: ', '').split(' ')
				missed.append((name, mode))
	print()
	for name, mode in missed:
		if not syms.get(name, None):
			print(f'Warning! Missed: {mode} {name}')
	print()
	return True


def create_general_function_sym_file(files, clean_flag, name):
	with open(os.path.join('libgen', name), 'w') as output:
		for file in files:
			with open(os.path.join('libgen', file), 'r') as input:
				output.write(f'# {file}\r\n')
				output.write(input.read())
				output.write('\r\n\r\n\r\n')
	if clean_flag:
		clean_files('libgen', files)


def determine_platfrom(base_address):
	if base_address == '0x10080000':
		return 'LTE1'
	elif base_address == '0x10092000':
		return 'LTE2'
	elif base_address == '0x100A0000':
		return 'LTE2'
	else:
		return 'UNKN'


def move_file(f, t):
	to = os.path.join('libgen', t)
	print(f'Moving file: "{f}" => "{to}".')
	os.replace(os.path.join(f), to)


def find_functions_from_patterns(cg1_path, pat_path, ram_trans_flag, base_address, output):
	result = 1
	if os.path.exists(cg1_path) and os.path.exists(pat_path):
		args = [
			PAT_UTILITY,
			ram_trans_flag,
			cg1_path,
			pat_path,
			base_address
		]
		print()
		result = subprocess.run(args).returncode
		command = ' '.join(args)
		print(f'Result of "{command}" command is "{result}".')
		print()
		if result == 0:
			move_file('functions.sym', output)
	else:
		if not os.path.exists(cg1_path):
			print(f'Cannot read "{cg1_path}" file!')
		else:
			print(f'Cannot read "{pat_path}" file!')
	return result == 0


def start_portkit_routines(ram_trans_flag, base_address, patterns, cg1):
	file_size = os.path.getsize(cg1)
	offset = (file_size & -16) + 16
	address = offset + int(base_address, 16)
	address_hex = f'0x{address:08X}'
	offset_hex = f'0x{offset:08X}'

	cg1_path = os.path.join(cg1)

	print(f'file_size={file_size}\noffset={offset_hex}\naddress={address_hex}')

	sym = 'all_functions.sym'
	output_file = 'general.sym'

	# Find SoC functions from patterns.
	platform = determine_platfrom(base_address)
	if platform == 'LTE1':
		pat_path = os.path.join('libgen', 'lte1.pat')
		find_functions_from_patterns(cg1_path, pat_path, '-no-ram-trans', base_address, 'lte1.sym')
	elif platform == 'LTE2':
		pat_path = os.path.join('libgen', 'lte2.pat')
		find_functions_from_patterns(cg1_path, pat_path, '-no-ram-trans', base_address, 'lte2.sym')
	else:
		print('Warning: Unknown platform!')
		output_file = sym

	pat_path = os.path.join('libgen', patterns)

	# Find general functions from patterns.
	find_functions_from_patterns(cg1_path, pat_path, ram_trans_flag, base_address, output_file)

	# Combine all into one big sym file.
	if platform == 'LTE1':
		create_general_function_sym_file(['general.sym', 'lte1.sym'], True, sym)
	elif platform == 'LTE2':
		create_general_function_sym_file(['general.sym', 'lte2.sym', 'lte2_irom.sym'], True, sym)

	# Validate one big sym file.
	res = validate_sym_file(sym)
	print(f'One big sym file validated, result={res}')
	if not res:
		return False

	# Find register address.
	generate_register_symbol_file(cg1_path, REGISTER_FUNCTION_INJECTION, sym, 'register')



if __name__ == '__main__':
	print('portkit.py script by EXL, 27-Nov-2023')
	print('ElfPack v1.0 PortKit Tool.')
	print()

	argc = len(sys.argv)
	if argc == 5:
		start_portkit_routines(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
	else:
		print('Usage:\n\t./portkit.py [RAM_TRANS] [BASE_ADDRESS] [function.pat] [CG1]')
		print()
		print('Example:\n\t./portkit.py -ram-trans 0x10080000 general.pat E1_R373_G_0E.30.49R.smg')
		print('\t./portkit.py -ram-trans 0x10092000 general.pat L7_R4513_G_08.B7.ACR_RB.smg')
		print('\t./portkit.py -no-ram-trans 0x100A0000 general.pat V3i_R4441D_G_08.01.03R.smg')
		print()
