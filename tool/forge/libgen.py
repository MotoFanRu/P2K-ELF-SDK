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
from datetime import datetime

from .constants import P2K_DIR_TOOL
from .constants import P2K_TOOL_POSTLINK
from .constants import P2K_SDK_CONSTS_H
from .constants import P2K_EP2_API_DEF
from .hexer import int2hex
from .types import LibraryModel
from .filesystem import check_files_if_exists
from .filesystem import check_files_extensions
from .filesystem import get_temporary_directory_path
from .filesystem import move_file
from .filesystem import compare_paths
from .filesystem import delete_file
from .symbols import validate_sym_file
from .symbols import dump_library_model_to_sym_file
from .symbols import dump_sym_file_to_library_model
from .invoker import invoke_external_command_res


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
		model: LibraryModel = dump_sym_file_to_library_model(p_sym_lib)
		if model is not None:
			model = ep1_libgen_model_sort(model, sort)
			entries: str = ''
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


def ep2_libgen_model_sort(model: LibraryModel, sort: LibrarySort) -> LibraryModel:
	if sort != LibrarySort.NONE:
		first_modes: set[str] = {'C'}

		def custom_sorting_function(entry: tuple[str, str, str]) -> tuple[int, str]:
			address, mode, name = entry
			priority: int = 0 if mode in first_modes else 1
			return priority, entry[sort.value].lower()

		return sorted(model, key=custom_sorting_function)
	return model


def ep2_libgen_model(p_sym_lib: Path, sort: LibrarySort) -> LibraryModel | None:
	if check_files_if_exists([p_sym_lib]):
		model: LibraryModel = dump_sym_file_to_library_model(p_sym_lib)
		model = ep2_libgen_model_sort(model, sort)
		return model
	return None


def ep2_libgen_version() -> str:
	# TODO: What is '1' in the end?
	return datetime.now().strftime('%d%m%y') + '1'


def ep2_libgen_library(p_sym: Path, sort: LibrarySort, firmware: str, p_out: Path) -> bool:
	is_library_sa: bool = check_files_extensions([p_out], ['sa'], False)
	is_library_bin: bool = check_files_extensions([p_out], ['bin'], False)
	if (not is_library_sa) and (not is_library_bin):
		logging.error(f'Unknown library type "{p_out}", should be "*.sa" or "*.bin" extension.')
		return False

	if check_files_if_exists([p_sym, P2K_SDK_CONSTS_H, P2K_EP2_API_DEF, P2K_TOOL_POSTLINK]):
		model: LibraryModel = ep2_libgen_model(p_sym, sort)
		if model is not None:
			result: bool = False
			sorted_sym_file: Path = get_temporary_directory_path() / 'Sorted.sym'
			phone_bin_library: Path = P2K_DIR_TOOL / 'std.lib'
			sdk_stub_sa_library: Path = P2K_DIR_TOOL / 'std.sa'
			if dump_library_model_to_sym_file(model, sorted_sym_file):
				if validate_sym_file(sorted_sym_file):
					args: list[str] = [
						str(P2K_TOOL_POSTLINK),
						'-stdlib', str(sorted_sym_file),
						'-def', str(P2K_EP2_API_DEF),
						'-fw', firmware,
						'-v', ep2_libgen_version(),
						'-header', str(P2K_SDK_CONSTS_H)
					]
					result = invoke_external_command_res([sorted_sym_file], args)
					if result and check_files_if_exists([phone_bin_library, sdk_stub_sa_library]):
						if is_library_bin:
							move_file(phone_bin_library, p_out, False)
						else:
							move_file(sdk_stub_sa_library, p_out, False)
					else:
						logging.error(f'Cannot create "{phone_bin_library}" and "{sdk_stub_sa_library}" libraries.')
			delete_file(sorted_sym_file, False)
			delete_file(phone_bin_library, False)
			if not compare_paths(sdk_stub_sa_library, p_out):
				delete_file(sdk_stub_sa_library, False)
			return result
	return False
