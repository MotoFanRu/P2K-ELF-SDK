# forge/patcher.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import shutil
import logging
import configparser

from pathlib import Path

from .hexer import int2hex
from .hexer import int2hex_r
from .hexer import hex2int_r
from .hexer import arrange16
from .hexer import normalize_hex_address


# Case-sensitive config parser.
class CsConfigParser(configparser.ConfigParser):
	def optionxform(self, option):
		return option


def patch_size_of_hex_str(hex_str: str) -> int:
	return int(len(hex_str) / 2)


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


def generate_fpa(fw: str, ath: str, dsc: str, p: dict[str, str], file: Path, u: dict[str, str] | None = None) -> bool:
	if len(p) > 0:
		with file.open(mode='w', newline='\r\n') as f_o:
			f_o.write('; This patch file was generated by the "forge" library.\n')
			f_o.write('[Patch_Info]\n')
			f_o.write(f'SW_Ver={fw}\n')
			f_o.write(f'Author={ath}\n')
			f_o.write(f'Description={dsc}\n')
			f_o.write('[Patch_Code]\n')
			for addr, value in p.items():
				f_o.write(f'{addr}: {value}\n')
			if (u is not None) and (len(u) > 0):
				f_o.write('[Patch_Undo]\n')
				for addr, undo in u.items():
					f_o.write(f'{addr}: {undo}\n')
		return True
	else:
		logging.error(f'Patch data is empty.')
		return False


def undo_data(addr: int, hex_data: str, undo: Path) -> str | None:
	if undo.is_file() and undo.exists():
		if undo.name.endswith('.bin') or undo.name.endswith('.smg'):
			with undo.open(mode='rb') as f_i:
				patch_size: int = patch_size_of_hex_str(hex_data)
				if not check_if_address_beyond_file_size(addr, undo.stat().st_size, patch_size):
					f_i.seek(addr)
					undo_str = f_i.read(patch_size).hex().upper()
					return undo_str
		else:
			logging.error(f'Check binary "*.bin" or "*.smg" undo source file extension.')
	else:
		logging.error(f'Undo source file {undo} is not exist.')
	return None


def bin2fpa(fw: str, author: str, desc: str, addr: int, binary: Path, fpa: Path, undo: Path | None = None) -> bool:
	if binary.is_file() and binary.exists():
		if binary.name.endswith('.bin'):
			with binary.open(mode='rb') as f_i:
				hex_str: str = f_i.read().hex().upper()
				return hex2fpa(fw, author, desc, addr, hex_str, fpa, undo)
		else:
			logging.error(f'Check binary "*.bin" file extension.')
	else:
		logging.error(f'Binary file {binary} is not exist.')
	return False


def hex2fpa(fw: str, author: str, desc: str, addr: int, hex_data: str, fpa: Path, undo: Path | None = None) -> bool:
	patch_size: int = patch_size_of_hex_str(hex_data)
	if patch_size % 2 == 0:
		patch_dict: dict[str, str] = {int2hex_r(addr): hex_data}
		if undo is not None:
			undo_dict: dict[str, str] = {int2hex_r(addr): undo_data(addr, hex_data, undo)}
			if undo_dict.get(int2hex_r(addr)) is not None:
				return generate_fpa(fw, author, desc, patch_dict, fpa, undo_dict)
		return generate_fpa(fw, author, desc, patch_dict, fpa)
	else:
		logging.error(f'Patch size "{patch_size}" must be even.')
	return False


def get_fpa_patch_values(config: CsConfigParser, section: str, is_code: bool = False) -> dict[str, str] | None:
	values = {}
	if config.has_section(section):
		for option in config[section]:
			value = config[section][option]
			if is_code:
				option = normalize_hex_address(option, True)
			values[option] = value
			logging.debug(f'{option} : {value}')
		return values
	return None


def fpa2bin(fpa: Path, binary: Path) -> bool:
	if binary.name.endswith('.bin'):
		config = CsConfigParser()
		config.read(fpa)
		values = get_fpa_patch_values(config, 'Patch_Code', True)
		if values is not None:
			for address, value in values:
				binary_chunk = binary.with_stem(f'{binary.stem}_{address}')
				with binary_chunk.open(mode='wb') as f_o:
					f_o.write(bytes.fromhex(value))
			return True
	else:
		logging.error(f'Check binary "*.bin" file extension.')
	return False


def unite_dicts_to_one(*dicts: dict[str, str]) -> dict[str, str] | None:
	united_dict: dict[str, str] = {}
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
	united_code_list: list[dict[str, str]] = []
	united_undo_list: list[dict[str, str]] = []
	for patch in patches:
		config = CsConfigParser()
		config.read(patch)
		code_p = get_fpa_patch_values(config, 'Patch_Code', True)
		if code_p is not None:
			united_code_list.append(code_p)
		undo_p = get_fpa_patch_values(config, 'Patch_Undo', True)
		if undo_p is not None:
			united_undo_list.append(undo_p)
	if len(united_code_list) > 0:
		united_code_dict = unite_dicts_to_one(*united_code_list)
		united_undo_dict = unite_dicts_to_one(*united_undo_list)
		united_code_sorted_dict = {key: united_code_dict[key] for key in sorted(united_code_dict)}
		united_undo_sorted_dict = {key: united_undo_dict[key] for key in sorted(united_undo_dict)}
		return generate_fpa(fw, author, desc, united_code_sorted_dict, result, united_undo_sorted_dict)
	return False


def apply_fpa_patch(firmware: Path, fpa: Path, backup: bool, validating: bool) -> bool:
	if firmware.is_file() and firmware.exists() and fpa.is_file() and fpa.exists():
		if firmware.name.endswith('.bin') or firmware.name.endswith('.smg'):
			if fpa.name.endswith('.fpa'):
				config = CsConfigParser()
				config.read(fpa)
				file_size = firmware.stat().st_size
				if validating:
					undo_patches = get_fpa_patch_values(config, 'Patch_Undo', True)
					if undo_patches is not None:
						with firmware.open(mode='rb') as f_i:
							for address, value in undo_patches.items():
								p_addr: int = hex2int_r(address)
								p_size: int = patch_size_of_hex_str(value)
								undo = value.upper()
								if check_if_address_beyond_file_size(p_addr, file_size, p_size):
									f_i.seek(hex2int_r(address))
									hex_data = f_i.read(patch_size_of_hex_str(value)).hex().upper()
								else:
									hex_data = 'FF' * p_size
								if hex_data == undo:
									logging.info(f'Patch "{address}" is validated.')
								else:
									logging.info(f'Patch "{address}" not valid with undo values {hex_data}=={undo}.')
									return False
				patches = get_fpa_patch_values(config, 'Patch_Code', True)
				if patches is not None:
					if backup:
						backup_file = firmware.with_stem(f'{firmware.stem}_backup')
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
								p_pad = (arrange16(file_size) - file_size) + arrange16(p_size)
								logging.info(f'Add some extra space "{int2hex(p_pad)}" to "{firmware} {file_size}".')
								f_o.write(b'\xFF' * p_pad)
								f_o.seek(p_addr)
								f_o.write(bytes.fromhex(value))
					return True
			else:
				logging.error(f'Check patch "*.fpa" file extension.')
		else:
			logging.error(f'Check firmware binary "*.bin" or "*.smg" file extension.')
	return False
