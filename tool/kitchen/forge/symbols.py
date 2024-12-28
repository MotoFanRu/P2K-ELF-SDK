# forge/symbols.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

import logging

from pathlib import Path

from .types import Symbol
from .hexer import hex2int
from .hexer import hex2hex
from .types import LibraryModel
from .constants import ADS_SYM_FILE_HEADER
from .filesystem import check_files_if_exists
from .filesystem import check_files_extensions
from .utilities import get_current_datetime_formatted


def split_and_validate_line(line: str) -> Symbol:
	try:
		modes: set[str] = {'A', 'C', 'D', 'T'}
		line: str = line.strip()
		if len(line) != 0 and not line.startswith('#'):
			address, mode, name = line.split()
			if mode != 'C':
				hex2int(address)
			if mode in modes:
				if len(name.strip()) >= 1:
					return address, mode, name
				else:
					raise ValueError(f'Too short or empty function name of "{address} {mode} {name}" entry.')
			else:
				raise ValueError(f'Unknown mode here: "{address} {mode} {name}", available only "{modes}" modes.')
	except ValueError as error:
		logging.debug(f'Parse error: "{error}".')
	return None, None, None


def create_combined_sym_file(files: list[Path], out_p: Path) -> bool:
	if check_files_if_exists(files) and check_files_extensions(files, ['sym']):
		with out_p.open(mode='w', newline='\r\n') as f_o:
			f_o.write(f'{ADS_SYM_FILE_HEADER}\n')
			f_o.write('# SYMDEFS ADS HEADER\n\n\n\n')
			for file in files:
				with file.open(mode='r') as f_i:
					f_o.write(f'# {file.name}\n')
					f_o.write(f_i.read())
					f_o.write('\n\n\n')
			return True
	return False


def validate_sym_file(in_p: Path) -> bool:
	if check_files_if_exists([in_p]) and check_files_extensions([in_p], ['sym']):
		symbols: dict[str, tuple[str, str]] = {}
		missed: list[tuple[str, str]] = []
		index: int = 0
		with in_p.open(mode='r') as f_i:
			for line in f_i.read().splitlines():
				line: str = line.strip()
				if (index == 0) and line != ADS_SYM_FILE_HEADER:
					logging.error(f'Symbols file "{in_p}" does not contains "{ADS_SYM_FILE_HEADER}" at first line.')
					return False
				if len(line) != 0 and not line.startswith('#'):
					address, mode, name = split_and_validate_line(line)
					if name is not None:
						if not symbols.get(name, None):
							symbols[name] = address, mode
						else:
							first_address, first_mode = symbols[name]
							if (first_mode == 'C') ^ (mode == 'C'):  # XOR here, CONST names may be same as other names.
								logging.warning(f'Duplicate SYM values in "{in_p}" symbols file:')
								logging.warning(f'\t{first_address} {first_mode} {name}')
								logging.warning(f'\t{address} {mode} {name}')
							else:
								logging.error(f'Duplicate SYM values in "{in_p}" symbols file:')
								logging.error(f'\t{first_address} {first_mode} {name}')
								logging.error(f'\t{address} {mode} {name}')
								return False
				elif line.startswith('# NOT_FOUND: '):
					mode, name = line.replace('# NOT_FOUND: ', '').split(' ')
					logging.debug(line)
					missed.append((name, mode))
				index += 1
		logging.info(f'Checking missing symbols in "{in_p}" file.')
		for name, mode in missed:
			if not symbols.get(name, None):
				logging.warning(f'Missed: {mode} {name}')
		return True
	return False


def get_function_address_from_sym_file(in_p: Path, func: str) -> int:
	with in_p.open(mode='r') as f_i:
		for line in f_i.read().splitlines():
			address, mode, name = split_and_validate_line(line)
			if name == func:
				return (int(address, 16) + 1) if mode == 'T' else int(address, 16)
	return 0x00000000


def dump_library_model_to_sym_file(model: LibraryModel, out_p: Path, phone: str, fw: str, ep: str, ver: str) -> bool:
	if len(model) > 0:
		try:
			with out_p.open(mode='w', newline='\r\n') as f_o:
				f_o.write(f'{ADS_SYM_FILE_HEADER}\n')
				f_o.write('# SYMDEFS ADS HEADER\n\n')
				f_o.write('# Symbol listing was generated by "forge" library.\n')
				f_o.write('# Source Code: https://github.com/MotoFanRu/P2K-ELF-SDK\n')
				f_o.write(f'# ElfPack: {ep}\n')
				f_o.write(f'# Phone: {phone}\n')
				f_o.write(f'# Firmware: {fw}\n')
				f_o.write(f'# Version: {ver}\n')
				f_o.write(f'# Timestamp: {get_current_datetime_formatted()}\n\n')
				for addr, mode, name in model:
					line: str = f'{addr} {mode} {name}'
					f_o.write(line + '\n')
					logging.debug(line)
				return True
		except OSError as error:
			logging.error(f'Cannot write "{out_p}" symbols file: {error}')
	else:
		logging.error(f'Library model of "{out_p}" is empty.')
	return False


def dump_sym_file_to_library_model(in_p: Path, validate: bool = False) -> LibraryModel | None:
	if not check_files_if_exists([in_p], False):
		return None
	if validate:
		if not validate_sym_file(in_p):
			return None
	try:
		model: LibraryModel = []
		with in_p.open(mode='r') as f_i:
			for line in f_i.read().splitlines():
				address, mode, name = split_and_validate_line(line)
				if (address is not None) and (mode is not None) and (name is not None):
					model.append((hex2hex(address, 8), mode, name))
			return model
	except OSError as error:
		logging.error(f'Cannot parse "{in_p}" symbols file: {error}')
	return None


def remove_comments_in_header_line(line: str) -> str:
	comment_offset: int = line.find('//')
	return line[:comment_offset].rstrip() if (comment_offset != -1) else line


def parse_sdk_const_header_to_list(in_p: Path) -> list[tuple[str, str]] | None:
	if check_files_if_exists([in_p]) and check_files_extensions([in_p], ['h']):
		list_const_defines: list[tuple[str, str]] = []
		with in_p.open(mode='r') as f_i:
			for line in f_i.read().splitlines():
				line = line.strip()
				if line.startswith('#define'):
					line = remove_comments_in_header_line(line)
					try:
						define, name, index = line.split()
						logging.debug(f'Good parse "{line}".')
						list_const_defines.append((name, index))
					except ValueError:
						logging.debug(f'Fail parse "{line}".')

		# Validation.
		set_seen_name: set[str] = set()
		set_seen_index: set[str] = set()
		list_name_duplicates: list[str] = []
		list_index_duplicates: list[str] = []
		for name, index in list_const_defines:
			if name in set_seen_name:
				list_name_duplicates.append(name)
			else:
				set_seen_name.add(name)
			if index in set_seen_index:
				list_index_duplicates.append(index)
			else:
				set_seen_index.add(index)
		if len(list_name_duplicates) > 0:
			logging.error(f'Duplicate names found: "{list_name_duplicates}".')
			return None
		if len(list_index_duplicates) > 0:
			logging.error(f'Duplicate indexes found: "{list_name_duplicates}".')
			return None

		return list_const_defines
	return None


def combine_sym_str(addr: str, mode: str, name: str) -> str:
	return f'{addr} {mode} {name}'


def replace_syms(patches: list[str], in_p: Path, phone: str, firmware: str, ep: str, version: str) -> bool:
	if check_files_if_exists([in_p]) and check_files_extensions([in_p], ['sym']):
		model_patches: LibraryModel = []
		for patch in patches:
			addr, mode, name = split_and_validate_line(patch)
			if (addr is not None) and (mode is not None) and (name is not None):
				model_patches.append((addr, mode, name))
		if len(model_patches) > 0:
			model_library_patched: LibraryModel = []
			model_library_original: LibraryModel = dump_sym_file_to_library_model(in_p, True)
			if not model_library_original:
				logging.error('Original library model is empty.')
				return False

			# Apply all patches.
			for addr_original, mode_original, name_original in model_library_original:
				index: int = 0
				for addr_patch, mode_patch, name_patch in model_patches:
					if name_patch.strip() == name_original.strip():
						patched_sym: str = combine_sym_str(addr_patch, mode_patch, name_patch)
						original_sym: str = combine_sym_str(addr_original, mode_original, name_original)
						if (addr_patch != addr_original) or (mode_patch != mode_original):
							logging.info(f'Will apply "{original_sym}" => "{patched_sym}" patch.')
							model_library_patched[index] = (addr_patch, mode_patch, name_patch)
						else:
							logging.warning(f'Patch "{original_sym}" => "{patched_sym}" already applied.')
					else:
						name_is_present: bool = False
						for addr_patched, mode_patched, name_patched in model_library_patched:
							if name_original.strip() == name_patched.strip():
								name_is_present = True
						if not name_is_present:
							model_library_patched.append((addr_original, mode_original, name_original))
					index = len(model_library_patched) - 1

			# Add missing patches as symbols.
			for addr_patch, mode_patch, name_patch in model_patches:
				name_is_present: bool = False
				for addr_original, mode_original, name_original in model_library_original:
					if name_original.strip() == name_patch.strip():
						name_is_present = True
				if not name_is_present:
					patched_sym: str = combine_sym_str(addr_patch, mode_patch, name_patch)
					logging.info(f'Will add "{patched_sym}" patch as a symbol.')
					model_library_patched.append((addr_patch, mode_patch, name_patch))

			if len(model_library_patched) > 0:
				return dump_library_model_to_sym_file(model_library_patched, in_p, phone, firmware, ep, version)
			else:
				logging.error('Patched library model is empty.')
		else:
			logging.error('List of patches is empty.')

	return False
