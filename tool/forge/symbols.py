# forge/symbols.py
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

from .types import Symbol
from .hexer import hex2int
from .types import LibraryModel
from .constants import ADS_SYM_FILE_HEADER
from .utilities import get_current_datetime_formatted


def split_and_validate_line(line: str) -> Symbol:
	try:
		modes: set[str] = {'A', 'C', 'D', 'T'}
		line: str = line.strip()
		if len(line) != 0 and not line.startswith('#'):
			address, mode, name = line.split(' ')
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


def create_combined_sym_file(files: list[Path], out_p: Path) -> None:
	with out_p.open(mode='w', newline='\r\n') as f_o:
		f_o.write(f'{ADS_SYM_FILE_HEADER}\n')
		f_o.write(f'# SYMDEFS ADS HEADER\n\n\n\n')
		for file in files:
			with file.open(mode='r') as f_i:
				f_o.write(f'# {file.name}\n')
				f_o.write(f_i.read())
				f_o.write(f'\n\n\n')


def validate_sym_file(in_p: Path) -> bool:
	symbols: dict[str, str] = {}
	missed: list[tuple[str, str]] = []
	index: int = 0
	with in_p.open(mode='r') as f_i:
		for line in f_i.read().splitlines():
			line: str = line.strip()
			if (index == 0) and line != ADS_SYM_FILE_HEADER:
				logging.error(f'Symbols file does not contains "{ADS_SYM_FILE_HEADER}" at first line.')
				return False
			if len(line) != 0 and not line.startswith('#'):
				address, mode, name = split_and_validate_line(line)
				if name is not None:
					if not symbols.get(name, None):
						symbols[name] = address
					else:
						first_address: str = symbols[name]
						logging.error(f'Duplicate SYM values:')
						logging.error(f'\t{first_address} {mode} {name}')
						logging.error(f'\t{address} {mode} {name}')
						return False
			elif line.startswith('# NOT_FOUND: '):
				mode, name = line.replace('# NOT_FOUND: ', '').split(' ')
				logging.debug(line)
				missed.append((name, mode))
			index += 1
	logging.info(f'')
	for name, mode in missed:
		if not symbols.get(name, None):
			logging.warning(f'Warning! Missed: {mode} {name}')
	logging.info(f'')
	return True


def get_function_address_from_sym_file(in_p: Path, func: str) -> int:
	with in_p.open(mode='r') as f_i:
		for line in f_i.read().splitlines():
			address, mode, name = split_and_validate_line(line)
			if name == func:
				return (int(address, 16) + 1) if mode == 'T' else int(address, 16)
	return 0x00000000


def dump_library_model_to_sym_file(model: LibraryModel, out_p: Path) -> bool:
	if len(model) > 0:
		try:
			with out_p.open(mode='w', newline='\r\n') as f_o:
				f_o.write(f'{ADS_SYM_FILE_HEADER}\n')
				f_o.write(f'# SYMDEFS ADS HEADER\n\n')
				f_o.write(f'# Symbol listing was generated by "forge" library.\n')
				f_o.write(f'# Timestamp: {get_current_datetime_formatted()}\n\n')
				for addr, mode, name in model:
					line: str = f'{addr} {mode} {name}'
					f_o.write(line + '\n')
					logging.debug(line)
				return True
		except OSError as error:
			logging.error(f'Cannot write "{out_p}" symbols file: {error}')
	else:
		logging.error(f'Library model is empty.')
	return False


def dump_sym_file_to_library_model(in_p: Path) -> LibraryModel | None:
	try:
		model: LibraryModel = []
		with in_p.open(mode='r') as f_i:
			for line in f_i.read().splitlines():
				address, mode, name = split_and_validate_line(line)
				if (address is not None) and (mode is not None) and (name is not None):
					model.append((address, mode, name))
			return model
	except OSError as error:
		logging.error(f'Cannot parse "{in_p}" symbols file: {error}')
	return None
