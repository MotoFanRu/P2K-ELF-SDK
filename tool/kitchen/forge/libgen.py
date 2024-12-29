# forge/libgen.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

import re
import logging

from pathlib import Path
from datetime import datetime
from argparse import Namespace

from .constants import P2K_DIR_LIB
from .constants import P2K_EP2_NMS_DEF
from .constants import P2K_EP2_API_DEF
from .constants import P2K_SDK_CONSTS_H
from .constants import P2K_TOOL_POSTLINK
from .constants import P2K_DIR_TOOL_KITCHEN
from .constants import P2K_ARGONLV_PHONES
from .hexer import int2hex
from .hexer import hex2int
from .hexer import hex2hex
from .types import LibraryModel
from .types import NamesDefs
from .filesystem import check_files_if_exists
from .filesystem import check_files_extensions
from .filesystem import get_all_directories_in_directory
from .filesystem import get_temporary_directory_path
from .filesystem import move_file
from .filesystem import compare_paths
from .filesystem import delete_file
from .firmware import parse_phone_firmware
from .symbols import validate_sym_file
from .symbols import replace_syms
from .symbols import split_and_validate_line
from .symbols import dump_sym_file_to_library_model
from .symbols import dump_library_model_to_sym_file
from .symbols import parse_sdk_const_header_to_list
from .invoker import invoke_external_command_res
from .types import ElfPack
from .types import LibrarySort


def libgen_version() -> str:
	# TODO: What is '1' in the end? Epoch?
	return datetime.now().strftime('%d%m%y') + '1'


def ep1_normalize_address(address: int, argonlv: bool) -> tuple[str, int]:
	data_shift: int = 0xC0000000 if argonlv else 0x30000000
	if address == 0xFFFFFFFF:
		return 'D', 0xFFFFFFFF
	if address > data_shift:
		return 'D', (address - data_shift)
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
			if len(model) > 0 and len(entries.strip()) > 0:
				return entries, model
	return None


def ep1_libgen_library(p_bin_lib: Path, model: LibraryModel, functions: str, argonlv: bool) -> bool:
	entry_count: int = len(model)
	data_shift: int = 0xC0000000 if argonlv else 0x30000000
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
						address_int += data_shift
					f_o.write(offset.to_bytes(4, byteorder='big'))
					try:
						address_value_to_write = address_int.to_bytes(4, byteorder='big')
					except OverflowError:
						raw_address_int: int = 0xFFFFFFFF
						logging.warning(f'32-bit int overflow on "{address} {mode} {name}" line.')
						logging.warning(f'Overflowed value: {int2hex(address_int)}')
						logging.warning(f'Will write default value: {int2hex(raw_address_int)}')
						address_value_to_write = raw_address_int.to_bytes(4, byteorder='big')
					f_o.write(address_value_to_write)
				else:
					logging.error(f'Function "{address_int} {mode} {name}" not found in the library model.')

			for func in functions.split(' '):
				func: str = func.strip()
				if len(func) > 0:
					f_o.write(func.encode('utf-8'))
					f_o.write(0x00.to_bytes(1, byteorder='big'))

			return True
	else:
		logging.error('Library model is empty.')
	return False


def ep1_libgen_asm(
	p_asm_src: Path,
	model: LibraryModel,
	gcc_asm: bool = False, gcc_equ: bool = False, fake_adresses = True
) -> bool:
	header_ads: str = """
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

	header_gcc: str = """
.syntax unified
.arm

.align 4

.section .text._start
.global _start
.type _start, %function
_start:
	stmfd   sp!, {r4-r11, lr}
	ldr     r12, =Register
	mov     lr, pc
	bx      r12
	ldmfd   sp!, {r4-r11, lr}
	bx      lr
	.ltorg

"""

	header_gcc_equ: str = """
.syntax unified
"""

	function_section_ads: str = """
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

	function_section_gcc: str = """
.section .text.{0}
.thumb
.thumb_func
.type {0}, %function
{0}:
	bx pc

.arm
.type {0}32, %function
{0}32:
	ldr   r12, ={1}
	bx    r12
	.ltorg
"""

	function_section_gcc_equ: str = """
.global {0}
.type {0}, %function
.equ {0}, {1} | {2}
"""

	data_section_ads: str = """
	AREA |a.{0}|, DATA, READONLY
{0}
	DCD    {1}
"""

	data_section_gcc: str = """
.section .data.{0}
.type {0}, %common
{0}:
	.long {1}
"""

	data_section_gcc_equ: str = """
.global {0}
.equ {0}, {1}
"""

	import_section_ads: str = '\tEXPORT {0}\n'
	import_section_gcc: str = '.global {0}\n'
	end_section_ads: str = '\tEND\n'
	end_section_gcc: str = '\n@END\n'

	header: str = header_ads
	function_section: str = function_section_ads
	data_section: str = data_section_ads
	import_section: str = import_section_ads
	end_section: str = end_section_ads
	if gcc_asm or gcc_equ:
		import_section = import_section_gcc
		end_section = end_section_gcc
	if gcc_asm:
		header = header_gcc
		function_section = function_section_gcc
		data_section = data_section_gcc
	if gcc_equ:
		header = header_gcc_equ
		function_section = function_section_gcc_equ
		data_section = data_section_gcc_equ

	offset_start: int = 0x10080000
	exports: list[str] = []
	header: str = header.replace('\n', '', 1)
	offset_start += 1
	entry_count: int = len(model)
	if entry_count > 0:
		with p_asm_src.open(mode='w', newline='\r\n') as f_o:
			f_o.write(header)

			for address, mode, name in model:
				entity_address: int = int2hex(offset_start) if fake_adresses else address
				if mode == 'D':
					exports.append(name)
					f_o.write(data_section.format(name, entity_address))
				else:
					exports.append(name)
					exports.append(name + '32')
					if gcc_equ:
						f_o.write(function_section.format(name, entity_address, 1 if mode == 'T' else 0))
					else:
						f_o.write(function_section.format(name, entity_address))
				offset_start += 4

			if not gcc_equ:
				f_o.write('\n\n\n\n')
				for export in exports:
					f_o.write(import_section.format(export))
				f_o.write('\n\n')
				f_o.write(end_section)
		return True
	return False


def ep1_libgen_symbols(p_lib: Path, p_sym: Path, sort: LibrarySort, phone: str, fw: str) -> bool:
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
					mode, address = ep1_normalize_address(ent[i][1], phone in P2K_ARGONLV_PHONES)  # Second is address.
					entry: tuple[str, str, str] = (int2hex(address), mode, ent_names[i])
					if address == 0xFFFFFFFF:
						logging.warning(f'Overflowed value on "{entry}" entry.')
					model.append(entry)
				model = ep1_libgen_model_sort(model, sort)
				if dump_library_model_to_sym_file(model, p_sym, phone, fw, 'EP1', libgen_version()):
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


def ep2_libgen_library(p_sym: Path, sort: LibrarySort, phone: str, firmware: str, p_out: Path) -> bool:
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
			phone_bin_library: Path = P2K_DIR_TOOL_KITCHEN / 'std.lib'
			sdk_stub_sa_library: Path = P2K_DIR_TOOL_KITCHEN / 'std.sa'
			version: str = libgen_version()
			if dump_library_model_to_sym_file(model, sorted_sym_file, phone, firmware, 'EP2', version):
				if validate_sym_file(sorted_sym_file):
					args: list[str] = [
						str(P2K_TOOL_POSTLINK),
						'-stdlib', str(sorted_sym_file),
						'-def', str(P2K_EP2_API_DEF),
						'-fw', firmware,
						'-v', version,
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


def ep2_libgen_header(p_lib: Path) -> tuple[dict[str, any], int] | None:
	if check_files_if_exists([p_lib]) and check_files_extensions([p_lib], ['bin']):
		header: dict[str, any] = {}
		magic: int = 0x7F4C4942
		with p_lib.open(mode='rb') as f_i:
			header['magic'] = int2hex(int.from_bytes(f_i.read(4), byteorder='big'))      # sizeof(uint32_t)
			header['version'] = f_i.read(1 * 8).decode('ascii').strip('\0')              # sizeof(char) * 8
			header['firmware'] = f_i.read(1 * 24).decode('ascii').strip('\0')            # sizeof(char) * 24
			header['symCnt'] = int.from_bytes(f_i.read(4), byteorder='big')              # sizeof(uint32_t)
			header['strTabSz'] = int.from_bytes(f_i.read(4), byteorder='big')            # sizeof(uint32_t)
			header['strTabOff'] = int.from_bytes(f_i.read(4), byteorder='big')           # sizeof(uint32_t)
			header['constCnt'] = int.from_bytes(f_i.read(4), byteorder='big')            # sizeof(uint32_t)
			header['constOff'] = int.from_bytes(f_i.read(4), byteorder='big')            # sizeof(uint32_t)

			logging.info('Library Header:')
			for k, v in header.items():
				logging.info(f'\t{k}={v}')

			lib_magic: int = hex2int(header.get('magic'))
			if lib_magic != magic:
				logging.error(f'Library magic "{int2hex(lib_magic)}" should be "{magic}".')
			else:
				return header, f_i.tell()
	return None


def ep2_libgen_names_defines(a_mode: str) -> NamesDefs | None:
	names_def: Path = P2K_EP2_NMS_DEF
	if check_files_if_exists([names_def]) and check_files_extensions([names_def], ['def']):
		list_names_def: NamesDefs = {}
		with names_def.open(mode='r') as f_i:
			for line in f_i.read().splitlines():
				addr, mode, name = split_and_validate_line(line)
				if (addr is not None) and (mode is not None) and (name is not None):
					if mode == a_mode:
						list_names_def[name] = addr
			if len(list_names_def) > 0:
				return list_names_def
	return None


def ep2_libgen_entries(list_ia: list[tuple[int, int]], list_na: list[str], resolve_names: bool) -> LibraryModel | None:
	len_ia: int = len(list_ia)
	len_na: int = len(list_na)
	if len_ia == len_na:
		names: NamesDefs = {}
		chunk_model: LibraryModel = []
		if resolve_names:
			names = ep2_libgen_names_defines('D')
		for i in range(0, len_ia, 1):
			address: int = list_ia[i][1]
			mode: str = 'A'
			name: str = list_na[i]
			if address % 2 != 0:
				mode = 'T'
				address -= 1
			if resolve_names:
				for data_name, data_address in names.items():
					if name == data_name:
						mode = 'D'
						break
			chunk_model.append((int2hex(address), mode, name))
		return chunk_model
	return None


def ep2_libgen_const_entries(list_ci: list[int], list_cv: list[int], resolve_names: bool) -> LibraryModel | None:
	len_ci: int = len(list_ci)
	len_cv: int = len(list_cv)
	if len_ci == len_cv:
		names: NamesDefs = {}
		chunk_model: LibraryModel = []
		if resolve_names:
			names = ep2_libgen_names_defines('C')
		for i in range(0, len_ci, 1):
			c_index: int = list_ci[i]
			c_value: int = list_cv[i]
			name: str = 'WARNING_WARNING_WARNING_UNKNOWN_CONST_NAME_INDEX_' + int2hex(c_index)
			if resolve_names:
				for const_name, const_index in names.items():
					if hex2int(hex2hex(const_index, 4), 4) == c_index:
						name = const_name
						break
			else:
				name += int2hex(c_index, 4)
			chunk_model.append((int2hex(c_value), 'C', name))
		return chunk_model
	return None


def ep2_libgen_combine_model_chunks(list_models: list[LibraryModel]) -> LibraryModel | None:
	if len(list_models) > 0:
		model: LibraryModel = []
		for model_in_list in list_models:
			for entry in model_in_list:
				model.append(entry)
		return model
	return None


def ep2_libgen_symbols(p_lib: Path, p_sym: Path, phone: str, sort: LibrarySort, resolve_names: bool) -> bool:
	if check_files_if_exists([p_lib]) and check_files_extensions([p_lib], ['bin']):
		header, header_offset = ep2_libgen_header(p_lib)
		if header is not None:
			with p_lib.open(mode='rb') as f_i:
				entries_index_address: list[tuple[int, int]] = []
				entries_name: list[str] = []
				entries_const_index: list[int] = []
				entries_const_value: list[int] = []

				f_i.seek(header_offset)

				# Indexes and addresses.
				for i in range(header_offset, header['strTabOff'], 4 * 2):
					index: int = int.from_bytes(f_i.read(4), 'big')
					address: int = int.from_bytes(f_i.read(4), 'big')
					logging.debug(f'Library entry index and address: "{int2hex(index)} {int2hex(address)}".')
					entries_index_address.append((index, address))
				logging.info(f'Found {len(entries_index_address)} indexes and addresses.')

				# Entries names.
				name_entry: str = ''
				name_count: int = 0
				for i in range(header['strTabOff'], header['constOff'], 1):
					ch = f_i.read(1)
					if ch != b'\x00':
						name_entry += ch.decode('ascii')
					else:
						name_entry = name_entry.strip()
						logging.debug(f'Library entry name "{name_entry}".')
						entries_name.append(name_entry)
						name_count += 1
						name_entry = ''
				logging.info(f'Found {len(entries_name)} names.')

				# Validation #1.
				sc_1: int = header.get('symCnt')
				sc_2: int = len(entries_index_address)
				sc_3: int = len(entries_name)
				sc_4: int = name_count
				if not (sc_1 == sc_2 == sc_3 == sc_4):
					logging.error(f'Wrong size of index/address/name arrays: "{sc_1}", "{sc_2}", "{sc_3}", "{sc_4}".')
					return False

				# Constants.
				for i in range(0, header.get('constCnt'), 1):
					index: int = int.from_bytes(f_i.read(2), 'big')  # sizeof(uint16_t)
					logging.debug(f'Library const index "{int2hex(index)}".')
					entries_const_index.append(index)
				logging.info(f'Found {len(entries_const_index)} const indexes.')
				for i in range(0, header.get('constCnt'), 1):
					value: int = int.from_bytes(f_i.read(4), 'big')  # sizeof(uint32_t)
					logging.debug(f'Library const value "{int2hex(value)}".')
					entries_const_value.append(value)
				logging.info(f'Found {len(entries_const_value)} const values.')

				# Validation #2.
				sc_6: int = header.get('constCnt')
				sc_7: int = len(entries_const_index)
				sc_8: int = len(entries_const_value)
				if not (sc_6 == sc_7 == sc_8):
					logging.error(f'Wrong size of const_index/const_values arrays: "{sc_6}", "{sc_7}", "{sc_8}".')
					return False

				# Resolve real entries names and sort model.
				chk_n: LibraryModel = ep2_libgen_entries(entries_index_address, entries_name, resolve_names)
				chk_c: LibraryModel = ep2_libgen_const_entries(entries_const_index, entries_const_value, resolve_names)
				model: LibraryModel = ep2_libgen_combine_model_chunks([chk_n, chk_c])
				model = ep2_libgen_model_sort(model, sort)

				# Save model to symbols file.
				if dump_library_model_to_sym_file(model, p_sym, phone, header['firmware'], 'EP2', header['version']):
					return validate_sym_file(p_sym)
	return False


def ep2_libgen_check_library_model_from_sym_file(p_sym: Path) -> LibraryModel | None:
	if check_files_if_exists([p_sym]) and validate_sym_file(p_sym):
		logging.debug(f'Will parse "{p_sym}" to library model.')
		return dump_sym_file_to_library_model(p_sym)
	return None


def ep2_libgen_generate_names_defines(sort: LibrarySort, out_p: Path) -> bool:
	if check_files_extensions([out_p], ['def']):
		library_models: list[tuple[Path, LibraryModel]] = []
		unique_data_names: set[str] = set()
		const_names: list[tuple[Path, str]] = []
		directories: list[Path] = get_all_directories_in_directory(P2K_DIR_LIB, True)
		if directories is not None:
			for directory in directories:
				ep1_sym_file: Path = directory / 'elfloader.sym'
				ep2_sym_file: Path = directory / 'library.sym'
				ep1_model: LibraryModel = dump_sym_file_to_library_model(ep1_sym_file, True)
				if ep1_model is not None:
					logging.info(f'Will add "{ep1_sym_file}" to library models list.')
					library_models.append((ep1_sym_file, ep1_model))
				ep2_model: LibraryModel = dump_sym_file_to_library_model(ep2_sym_file, True)
				if ep2_model is not None:
					logging.info(f'Will add "{ep2_sym_file}" to library models list.')
					library_models.append((ep2_sym_file, ep2_model))
		for path, model in library_models:
			for address, mode, name in model:
				if mode == 'C':
					const_names.append((path, name))
				elif mode == 'D':
					unique_data_names.add(name)

		names_def_model: LibraryModel = []
		for data_name in unique_data_names:
			names_def_model.append(('0xFFFFFFFF', 'D', data_name))

		const_header_names_indexes: list[tuple[str, str]] = parse_sdk_const_header_to_list(P2K_SDK_CONSTS_H)
		# Validation.
		for path, name in const_names:
			contains: bool = False
			for const_name, const_index in const_header_names_indexes:
				if const_name.strip() == name.strip():
					contains = True
					break
			if not contains:
				logging.error(f'Unknown const value: "{name}" in "{path}" symbols file')
				return False

		for const_name, const_index in const_header_names_indexes:
			const_name: str = const_name.strip()
			const_index: str = int2hex(hex2int(const_index, 4))
			names_def_model.append((const_index, 'C', const_name))

		names_def_model = ep2_libgen_model_sort(names_def_model, sort)
		if names_def_model is not None:
			try:
				with out_p.open(mode='w', newline='\r\n') as f_o:
					for addr, mode, name in names_def_model:
						line: str = f'{addr} {mode} {name}'
						f_o.write(line + '\n')
						logging.debug(line)
					return True
			except OSError as error:
				logging.error(f'Cannot write "{out_p}" names defines file, error: {error}')
	return False


def libgen_regenerator(sort: LibrarySort, e: ElfPack) -> bool:
	directories: list[Path] = get_all_directories_in_directory(P2K_DIR_LIB, True)
	if directories is not None:
		for directory in directories:
			if e == ElfPack.EP1:
				sym_file: Path = directory / 'elfloader.sym'
				lib_file: Path = directory / 'elfloader.lib'
			elif e == ElfPack.EP2:
				sym_file: Path = directory / 'library.sym'
				lib_file: Path = directory / 'library.bin'
			else:
				logging.error('Unknown ElfPack version.')
				return False

			# Drop all "_testX", "_testXX" slugs from name.
			pfw_chunk: str = re.sub(r'_test\d+', '', directory.name)
			phone, firmware = parse_phone_firmware(pfw_chunk, False)

			# Create Libraries.
			if check_files_if_exists([sym_file], False):
				if validate_sym_file(sym_file):
					logging.info(f'Will create "{lib_file}" library from "{sym_file}" symbols file.')
					if e == ElfPack.EP1:
						functions, library_model = ep1_libgen_model(sym_file, sort)
						if functions and library_model:
							if not ep1_libgen_library(lib_file, library_model, functions, phone in P2K_ARGONLV_PHONES):
								return False
						else:
							return False
					elif e == ElfPack.EP2:
						if not ep2_libgen_library(sym_file, sort, phone, firmware, lib_file):
							return False
				else:
					logging.error(f'Cannot open and check "{sym_file}" symbols file.')
					return False

			# Create Symbols files.
			if check_files_if_exists([lib_file], False):
				logging.info(f'Will create "{sym_file}" symbols file from "{lib_file}" library.')
				if e == ElfPack.EP1:
					if not ep1_libgen_symbols(lib_file, sym_file, sort, phone, firmware):
						return False
				elif e == ElfPack.EP2:
					if not ep2_libgen_symbols(lib_file, sym_file, phone, sort, True):
						return False
		return True
	return False


def ep1_libgen_regenerator(sort: LibrarySort) -> bool:
	return libgen_regenerator(sort, ElfPack.EP1)


def ep2_libgen_regenerator(sort: LibrarySort) -> bool:
	return libgen_regenerator(sort, ElfPack.EP2)


def libgen_resort_syms(sort: LibrarySort, e: ElfPack) -> bool:
	directories: list[Path] = get_all_directories_in_directory(P2K_DIR_LIB, True)
	if directories is not None:
		for directory in directories:
			if e == ElfPack.EP1:
				sym_file: Path = directory / 'elfloader.sym'
			elif e == ElfPack.EP2:
				sym_file: Path = directory / 'library.sym'
			else:
				logging.error(f'Unknown ElfPack version: "{e.name}".')
				return False

			# Drop all "_testX", "_testXX" slugs from name.
			pfw_chunk: str = re.sub(r'_test\d+', '', directory.name)
			phone, firmware = parse_phone_firmware(pfw_chunk, False)
			version: str = libgen_version()

			# Resort Symbols files.
			if check_files_if_exists([sym_file], False):
				logging.info(f'Will resort "{sym_file}" symbols file.')
				if e == ElfPack.EP1:
					functions, library_model = ep1_libgen_model(sym_file, sort)
					elfpack: str = ElfPack.EP1.name
				elif e == ElfPack.EP2:
					library_model: LibraryModel = ep2_libgen_model(sym_file, sort)
					elfpack: str = ElfPack.EP2.name
				else:
					return False

				if library_model:
					if not dump_library_model_to_sym_file(library_model, sym_file, phone, firmware, elfpack, version):
						return False

				if not validate_sym_file(sym_file):
					return False
		return True
	return False


def ep1_libgen_resort(sort: LibrarySort) -> bool:
	return libgen_resort_syms(sort, ElfPack.EP1)


def ep2_libgen_resort(sort: LibrarySort) -> bool:
	return libgen_resort_syms(sort, ElfPack.EP2)


def libgen_chunk_sym(i: Path, o: Path, sort: LibrarySort, symbols: list[str], pfw: tuple[str, str], e: ElfPack) -> bool:
	if check_files_if_exists([i], False) and check_files_extensions([i], ['sym']):
		if e == ElfPack.EP1:
			functions, library_model = ep1_libgen_model(i, sort)
		elif e == ElfPack.EP2:
			library_model: LibraryModel = ep2_libgen_model(i, sort)
		else:
			logging.error(f'Unknown ElfPack version: "{e.name}".')
			return False

		chunk_model: LibraryModel = []
		for addr, mode, name in library_model:
			if name in symbols:
				entry: tuple[str, str, str] = (addr, mode, name)
				logging.debug(f'Will append "({addr}, {mode}, {name})" => "{entry}".')
				chunk_model.append(entry)

		if chunk_model:
			phone, firmware = pfw
			if not dump_library_model_to_sym_file(chunk_model, o, phone, firmware, e.name, libgen_version()):
				return False

		return validate_sym_file(o)

	return False


def ep1_libgen_chunk_sym(i: Path, o: Path, sort: LibrarySort, symbols: list[str], pfw: tuple[str, str]) -> bool:
	return libgen_chunk_sym(i, o, sort, symbols, pfw, ElfPack.EP1)


def ep2_libgen_chunk_sym(i: Path, o: Path, sort: LibrarySort, symbols: list[str], pfw: tuple[str, str]) -> bool:
	return libgen_chunk_sym(i, o, sort, symbols, pfw, ElfPack.EP2)


def libgen_apply_patches(patches: list[str], io_sym: Path, phone: str, firmware: str, ep: str) -> bool:
	return replace_syms(patches, io_sym, phone, firmware, ep, libgen_version())


def libgen_names_sym(i: Path, e: ElfPack, ignore_consts: bool) -> list[str] | None:
	if check_files_if_exists([i]):
		if e == ElfPack.EP1:
			functions, library_model = ep1_libgen_model(i, LibrarySort.NAME)
		elif e == ElfPack.EP2:
			library_model: LibraryModel = ep2_libgen_model(i, LibrarySort.NAME)
		else:
			logging.error(f'Unknown ElfPack version: "{e.name}".')
			return None
		names: list[str] = [name for addr, mode, name in library_model if not ignore_consts or mode != 'C']
		if len(names) > 0:
			return names
		else:
			logging.error(f'Names array of "{i}" file is empty.')
	return None


def ep1_libgen_names_sym(i: Path) -> list[str] | None:
	return libgen_names_sym(i, ElfPack.EP1, False)


def ep2_libgen_names_sym(i: Path, ignore_consts: bool) -> list[str] | None:
	return libgen_names_sym(i, ElfPack.EP2, ignore_consts)


def determine_sort_mode(args: Namespace) -> LibrarySort:
	if args.sort_name:
		return LibrarySort.NAME
	elif args.sort_address:
		return LibrarySort.ADDR
	elif args.sort_type:
		return LibrarySort.MODE
	return LibrarySort.NONE


def libgen_get_library_sym(pfw: tuple[str, str], library_name: str) -> Path | None:
	library: Path = P2K_DIR_LIB / '_'.join(pfw) / library_name
	if check_files_if_exists([library]):
		return library
	return None


def ep1_libgen_get_library_sym(pfw: tuple[str, str]) -> Path | None:
	return libgen_get_library_sym(pfw, 'elfloader.sym')


def ep2_libgen_get_library_sym(pfw: tuple[str, str]) -> Path | None:
	return libgen_get_library_sym(pfw, 'library.sym')


def libgen_gcc_sym(model: LibraryModel, p_out: Path, c_source: False) -> bool:
	entry: str = '#define ADDR_{0:<40} = ({1} | {2}) /* {3} */' if c_source else '{0:<40} = ({1} | {2}); /* {3} */'
	entry_count: int = len(model)
	if entry_count > 0:
		with p_out.open(mode='w', newline='\r\n') as f_o:
			for address, mode, name in model:
				f_o.write(entry.format(name, address, mode))
		return True
	return False
