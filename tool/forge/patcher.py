# forge/patcher.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

import shutil
import logging

from pathlib import Path

from .hexer import int2hex
from .hexer import int2hex_r
from .hexer import hex2int_r
from .hexer import arrange16
from .types import PatchDict
from .types import PatchDictNone
from .hexer import normalize_hex_address
from .filesystem import check_files_if_exists
from .filesystem import check_files_extensions
from .types import CsConfigParser


def sort_patch_dict(unsorted: PatchDict) -> PatchDict:
	return {key: unsorted[key] for key in sorted(unsorted)}


def patch_size_of_hex_str(hex_str: str) -> int:
	length: int = len(hex_str)
	length_good: int = int(len(hex_str) / 2)
	if length % 2 != 0:
		logging.warning(f'Wrong and odd size "{length}" of patch!')
		logging.warning(f'Will truncate it to "{length}/2={length_good}" size!')
	return length_good


def check_if_address_beyond_file_size(address: int, file_size: int, patch_size: int) -> bool:
	offset_size: int = address + patch_size
	if (address > file_size) or (offset_size > file_size):
		e_a: str = int2hex(address)
		e_p: str = int2hex(patch_size)
		e_o: str = int2hex(offset_size)
		e_f: str = int2hex(file_size)
		logging.warning(f'Read/Write position beyond: "addr={e_a}", "patch={e_p}" "offset={e_o}", "file_size={e_f}"')
		return False
	return True


def generate_fpa(fw: str, author: str, desc: str, patterns: PatchDict, fpa: Path, undos: PatchDictNone = None) -> bool:
	if len(patterns) > 0:
		with fpa.open(mode='w', newline='\r\n') as f_o:
			f_o.write('; This patch file was generated by the "forge" library.\n')
			f_o.write('[Patch_Info]\n')
			f_o.write(f'SW_Ver={fw}\n')
			f_o.write(f'Author={author}\n')
			f_o.write(f'Description={desc}\n')
			f_o.write('[Patch_Code]\n')
			for addr, value in patterns.items():
				f_o.write(f'{addr}: {value}\n')
			if (undos is not None) and (len(undos) > 0):
				f_o.write('[Patch_Undo]\n')
				for addr, undo in undos.items():
					f_o.write(f'{addr}: {undo}\n')
		return True
	else:
		logging.error(f'Patch data is empty.')
		return False


def undo_data(addr: int, hex_data: str, undo: Path) -> str | None:
	if check_files_if_exists([undo]) and check_files_extensions([undo], ['bin', 'smg']):
		with undo.open(mode='rb') as f_i:
			patch_size: int = patch_size_of_hex_str(hex_data)
			if check_if_address_beyond_file_size(addr, undo.stat().st_size, patch_size):
				f_i.seek(addr)
				undo_str: str = f_i.read(patch_size).hex().upper()
			else:
				undo_str: str = 'FF' * patch_size
			return undo_str
	return None


def bin2fpa(fw: str, author: str, desc: str, addr: int, binary: Path, fpa: Path, undo: Path | None = None) -> bool:
	if check_files_if_exists([binary]) and check_files_extensions([binary], ['bin']):
		with binary.open(mode='rb') as f_i:
			hex_str: str = f_i.read().hex().upper()
			return hex2fpa(fw, author, desc, addr, hex_str, fpa, undo)
	return False


def hex2fpa(fw: str, author: str, desc: str, addr: int, hex_data: str, fpa: Path, undo: Path | None = None) -> bool:
	patch_size: int = patch_size_of_hex_str(hex_data)
	if patch_size % 2 != 0:
		logging.warning(f'Patch size "{int2hex(patch_size)} {patch_size}" is not even.')
	hex_patch: str = hex_data[:(patch_size * 2)]  # Truncate patch size.
	new_size: int = patch_size_of_hex_str(hex_patch)
	if patch_size != new_size:
		logging.warning(f'Patch was truncated from "{patch_size}" to "{new_size}".')
	patch_dict: PatchDict = {int2hex_r(addr): hex_patch}
	if undo is not None:
		undo_dict: PatchDict = {int2hex_r(addr): undo_data(addr, hex_patch, undo)}
		if undo_dict.get(int2hex_r(addr)) is not None:
			return generate_fpa(fw, author, desc, patch_dict, fpa, undo_dict)
	return generate_fpa(fw, author, desc, patch_dict, fpa)


def get_fpa_patch_values(config: CsConfigParser, section: str, is_code: bool = False) -> PatchDictNone:
	values: PatchDict = {}
	if config.has_section(section):
		for option in config[section]:
			value: str = config[section][option]
			if is_code:
				option: str = normalize_hex_address(option, True)
			values[option] = value
			logging.debug(f'{option} : {value}')
		return values
	return None


def fpa2bin(fpa: Path, binary: Path) -> bool:
	if check_files_extensions([binary], ['bin']):
		config: CsConfigParser = CsConfigParser()
		config.read(fpa)
		values: PatchDictNone = get_fpa_patch_values(config, 'Patch_Code', True)
		if values is not None:
			for address, value in values.items():
				binary_chunk_path: Path = binary.with_stem(f'{binary.stem}_{address}')
				logging.info(f'Writing "{binary_chunk_path}" binary file.')
				with binary_chunk_path.open(mode='wb') as f_o:
					f_o.write(bytes.fromhex(value))
			return True
	return False


def unite_dicts_to_one(*dicts: PatchDict) -> PatchDictNone:
	united_dict: PatchDict = {}
	duplicates: set[str] = set()
	for d in dicts:
		for key, value in d.items():
			if key in united_dict:
				logging.error(f'Something wrong with patches, there is duplicate address values "{key}".')
				duplicates.add(key)
			united_dict[key] = value
	if (len(duplicates) > 0) or (len(united_dict) == 0):
		return None
	return united_dict


def unite_fpa_patches(fw: str, author: str, desc: str, patches: list[Path], result: Path) -> bool:
	united_code_list: list[PatchDict] = []
	united_undo_list: list[PatchDict] = []
	for patch in patches:
		config: CsConfigParser = CsConfigParser()
		config.read(patch)
		code_p: PatchDictNone = get_fpa_patch_values(config, 'Patch_Code', True)
		if code_p is not None:
			united_code_list.append(code_p)
		undo_p: PatchDictNone = get_fpa_patch_values(config, 'Patch_Undo', True)
		if undo_p is not None:
			united_undo_list.append(undo_p)
	if len(united_code_list) > 0:
		united_code_dict: PatchDictNone = unite_dicts_to_one(*united_code_list)
		united_undo_dict: PatchDictNone = unite_dicts_to_one(*united_undo_list)
		if united_code_dict is not None:
			united_code_dict: PatchDictNone = sort_patch_dict(united_code_dict)
		if united_undo_dict is not None:
			united_undo_dict: PatchDictNone = sort_patch_dict(united_undo_dict)
		return generate_fpa(fw, author, desc, united_code_dict, result, united_undo_dict)
	return False


def apply_fpa_patch(firmware: Path, fpa: Path, backup: bool, validating: bool) -> bool:
	files_here: bool = check_files_if_exists([firmware, fpa])
	extensions_ok: bool = check_files_extensions([firmware], ['bin', 'smg']) and check_files_extensions([fpa], ['fpa'])
	if files_here and extensions_ok:
		config: CsConfigParser = CsConfigParser()
		config.read(fpa)
		file_size: int = firmware.stat().st_size
		if validating:
			undo_patches: PatchDictNone = get_fpa_patch_values(config, 'Patch_Undo', True)
			if undo_patches is not None:
				with firmware.open(mode='rb') as f_i:
					for address, value in undo_patches.items():
						p_addr: int = hex2int_r(address)
						p_size: int = patch_size_of_hex_str(value)
						undo: str = value.upper()
						if check_if_address_beyond_file_size(p_addr, file_size, p_size):
							f_i.seek(hex2int_r(address))
							hex_data: str = f_i.read(patch_size_of_hex_str(value)).hex().upper()
						else:
							hex_data: str = 'FF' * p_size
						if hex_data == undo:
							logging.info(f'Patch "{address}" is validated.')
						else:
							logging.info(f'Patch "{address}" not valid with undo values {hex_data}=={undo}.')
							return False
		patches: PatchDictNone = get_fpa_patch_values(config, 'Patch_Code', True)
		if patches is not None:
			if backup:
				backup_file: Path = firmware.with_stem(f'{firmware.stem}_backup')
				logging.info(f'Create backup file from "{firmware}" to "{backup_file}".')
				shutil.copy(firmware, backup_file)
			with firmware.open(mode='r+b') as f_o:
				for address, value in patches.items():
					p_addr: int = hex2int_r(address)
					p_size: int = patch_size_of_hex_str(value)
					if check_if_address_beyond_file_size(p_addr, file_size, p_size):
						f_o.seek(p_addr)
						f_o.write(bytes.fromhex(value))
					else:
						f_o.seek(file_size)
						p_pad: int = (arrange16(file_size) - file_size) + arrange16(p_size)
						logging.info(f'Add some extra space "{int2hex(p_pad)}" to "{firmware} {file_size}".')
						f_o.write(b'\xFF' * p_pad)
						f_o.seek(p_addr)
						f_o.write(bytes.fromhex(value))
			return True
	return False
