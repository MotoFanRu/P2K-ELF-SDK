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

from enum import Enum
from pathlib import Path

from .hexer import int2hex
from .types import LibraryModel
from .filesystem import check_files_if_exists
from .symbols import split_and_validate_line
from .symbols import validate_sym_file
from .symbols import dump_library_model_to_sym_file


class LibrarySort(Enum):
	ADDR: int = 0
	MODE: int = 1
	NAME: int = 2
	NONE: int = 3


def ep1_normalize_address(address: int) -> tuple[str, int]:
	if address > 0x30000000:
		return 'D', (address - 0x30000000)
	else:
		if address % 2 == 0:
			return 'A', address
		else:
			return 'T', (address - 1)


def ep1_libgen_model_sort(model: LibraryModel, sort: LibrarySort) -> LibraryModel:
	if sort != LibrarySort.NONE:
		return sorted(model, key=lambda x: x[sort.value].lower())
	return model


def ep1_libgen_model(p_sym_lib: Path, sort: LibrarySort) -> tuple[str, LibraryModel] | None:
	if check_files_if_exists([p_sym_lib]):
		model: LibraryModel = []
		with p_sym_lib.open(mode='r') as f_i:
			for line in f_i.read().splitlines():
				address, mode, name = split_and_validate_line(line)
				if (name is not None) and (mode is not None) and (name is not None):
					model.append((address, mode, name))
		entries: str = ''
		model = ep1_libgen_model_sort(model, sort)
		for address, mode, name in model:
			logging.debug(f'{name} {mode} {address}')
			entries += ' ' + name
		entries += ' '
		return entries, model
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
					f_o.write(0x00.to_bytes(1, byteorder='big'))

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


def ep1_libgen_symbols(p_lib: Path, p_sym: Path, sort: LibrarySort) -> bool:
	if check_files_if_exists([p_lib]):
		with (p_lib.open(mode='rb') as f_i):
			cnt: int = int.from_bytes(f_i.read(4), byteorder='big')
			logging.info(f'Library entries count: {cnt}')
			ent: list[tuple[int, int]] = []
			for i in range(0, cnt):
				ent.append((int.from_bytes(f_i.read(4), byteorder='big'), int.from_bytes(f_i.read(4), byteorder='big')))
			data: bytes = f_i.read()
			ent_names: list[str] = [symbols.decode('ascii') for symbols in data.split(b'\x00') if symbols]
			len_e: int = len(ent)
			len_n: int = len(ent_names)
			logging.info(f'Library entries addresses: {len_e}')
			logging.info(f'Library entries names: {len_n}')
			if cnt == len_e == len_n:
				logging.info(f'Library is valid, "cnt={cnt}", "len_e={len_e}", "len_n={len_n}" are equal.')
				model: LibraryModel = []
				for i in range(0, cnt):
					mode, address = ep1_normalize_address(ent[i][1])  # Second is address.
					model.append((int2hex(address), mode, ent_names[i]))
				model = ep1_libgen_model_sort(model, sort)
				if dump_library_model_to_sym_file(model, p_sym):
					return validate_sym_file(p_sym)
				else:
					logging.error(f'Cannot create symbols "{p_sym}" file.')
			else:
				logging.error(f'Library is invalid, "cnt={cnt}", "len_e={len_e}", "len_n={len_n}" are not equal.')
	return False
