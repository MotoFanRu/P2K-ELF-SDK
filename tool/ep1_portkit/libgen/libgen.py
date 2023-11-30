#!/usr/bin/env python

"""
libgen.py script by EXL, 26-Nov-2023

Generate libraries by symdefs files.
"""

import os
import sys


def generate_library_model(sym, lib_model):
	string_functions = ''
	with open(os.path.join(sym), 'r') as input:
		for line in input.read().splitlines():
			line = line.strip()
			if len(line) != 0 and not line.startswith('#'):
				address, mode, name = line.split(' ')
				string_functions += ' ' + name
				lib_model.append((address, mode, name))
	string_functions += ' '
	return string_functions


def generate_library_binary(file_name, lib_model, string_functions):
	entry_count = len(lib_model)
	print(entry_count)
	if entry_count > 0:
		with open(os.path.join(file_name), 'wb') as output:
			output.write(entry_count.to_bytes(4, byteorder='big'))

			for address, mode, name in lib_model:
				offset = string_functions.find(' ' + name + ' ')
				address = int(address, 16)
				if mode == 'T':
					address += 0x00000001
				elif mode == 'D':
					address += 0x30000000
				output.write(offset.to_bytes(4, byteorder='big'))
				output.write(address.to_bytes(4, byteorder='big'))

			for func in string_functions.split(' '):
				func = func.strip()
				if len(func) > 0:
					output.write(func.encode('utf-8'))
					output.write(0x00.to_bytes(1, byteorder='little'))


def generate_asm(file_name, lib_model):
	offset_start = 0x10080000

	header = \
"""
    AREA Lib, CODE, READONLY
    ALIGN 4

    IMPORT  Register

    EXPORT  Lib

    CODE32
    ENTRY
    STMFD   SP!, {R4-R11, LR}
    LDR     R12, =Register
    MOV     LR, PC
    BX      R12
    LDMFD   SP!, {R4-R11, LR}
    BX      LR
    LTORG

"""

	function_section = \
"""
    AREA |f.{0}|, CODE, READONLY
    CODE16
{0}
    BX    PC
    CODE32
{0}32
    LDR   R12, ={1}
    BX    R12
    LTORG
"""

	data_section = \
"""
    AREA |a.{0}|, DATA, READONLY
{0}
    DCD    {1}
"""

	exports = [ ]

	header = header.replace('\n', '', 1)
	offset_start += 1

	entry_count = len(lib_model)
	print(entry_count)
	if entry_count > 0:
		with open(os.path.join(file_name), 'w') as output:
			output.write(header.replace('\n', '\r\n'))

			for address, mode, name in lib_model:
				if mode == 'D':
					exports.append(name)
					output.write(data_section.format(name, f'0x{offset_start:08X}').replace('\n', '\r\n'))
				else:
					exports.append(name)
					exports.append(name + '32')
					output.write(function_section.format(name, f'0x{offset_start:08X}').replace('\n', '\r\n'))
				offset_start += 4

			output.write('\r\n\r\n')
			output.write('\r\n\r\n')

			for export in exports:
				output.write(f'    EXPORT {export}\r\n')

			output.write('\r\n\r\n')
			output.write('    END\r\n')



# TODO: Warnings on NOT_FOUND, etc.

if __name__ == '__main__':
	print('libgen.py script by EXL, 29-Nov-2023')
	print('Generate libraries by symdefs files.')
	print()

	argc = len(sys.argv)
	if argc == 2:
		lib_model = [ ]
		string_functions = generate_library_model(sys.argv[1], lib_model)
		generate_library_binary('lib.bin', lib_model, string_functions)
		generate_asm('lib.asm', lib_model)
	else:
		print('Usage:\n\t./libgen.py [SYM_FILE]')
		print()
		print('Example:\n\t./libgen.py Lib.sym')
		print()
