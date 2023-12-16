# forge/libgen.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import logging

from pathlib import Path

from .hexer import int2hex
from .types import LibraryModel
from .files import check_files_if_exists
from .symbols import split_and_validate_line


def ep1_libgen_model(p_sym_lib: Path, model: LibraryModel) -> str | None:
	if check_files_if_exists([p_sym_lib]):
		entries: str = ''
		with p_sym_lib.open(mode='r') as f_i:
			for line in f_i.read().splitlines():
				address, mode, name = split_and_validate_line(line)
				if name is not None:
					entries += ' ' + name
					model.append((address, mode, name))
		entries += ' '
		return entries
	return None


def ep1_libgen_library(p_bin_lib: Path, model: LibraryModel, functions: str) -> bool:
	entry_count: int = len(model)
	if entry_count > 0:
		with p_bin_lib.open(mode='wb') as f_o:
			f_o.write(entry_count.to_bytes(4, byteorder='big'))

			for address, mode, name in model:
				offset: int = functions.find(' ' + name + ' ')
				if offset != -1:
					address_int: int = int(address, 16)
					if mode == 'T':
						address_int += 0x00000001
					elif mode == 'D':
						address_int += 0x30000000
					f_o.write(offset.to_bytes(4, byteorder='big'))
					f_o.write(address_int.to_bytes(4, byteorder='big'))
				else:
					logging.error(f'Function "{address_int} {mode} {name}" not found in the library model.')

			for func in functions.split(' '):
				func: str = func.strip()
				if len(func) > 0:
					f_o.write(func.encode('utf-8'))
					f_o.write(0x00.to_bytes(1, byteorder='little'))

			return True
	else:
		logging.error(f'Library model is empty.')
	return False


def ep1_libgen_asm(p_asm_src: Path, model: LibraryModel) -> bool:
	offset_start: int = 0x10080000

	header: str = """
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

	function_section: str = """
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

	data_section: str = """
	AREA |a.{0}|, DATA, READONLY
{0}
	DCD    {1}
"""

	exports: list[str] = []
	header: str = header.replace('\n', '', 1)
	offset_start += 1
	entry_count: int = len(model)
	if entry_count > 0:
		with p_asm_src.open(mode='w', newline='\r\n') as f_o:
			f_o.write(header)

			for address, mode, name in model:
				if mode == 'D':
					exports.append(name)
					f_o.write(data_section.format(name, int2hex(offset_start)))
				else:
					exports.append(name)
					exports.append(name + '32')
					f_o.write(function_section.format(name, int2hex(offset_start)))
				offset_start += 4

			f_o.write('\n\n\n\n')

			for export in exports:
				f_o.write(f'\tEXPORT {export}\n')

			f_o.write('\n\n')
			f_o.write('\tEND\n')
		return True
	return False
