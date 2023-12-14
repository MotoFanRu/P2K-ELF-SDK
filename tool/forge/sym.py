# forge/sym.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru developers
"""

import logging

from pathlib import Path

from .hexer import hex2int
from .const import ADS_SYM_FILE_HEADER


def split_and_validate_line(line: str) -> tuple[str | None, str | None, str | None]:
	try:
		line = line.strip()
		if len(line) != 0 and not line.startswith('#'):
			address, mode, name = line.split(' ')
			hex2int(address)
			if mode == 'A' or mode == 'T' or mode == 'D' or mode == 'C':
				if len(name) >= 1:
					return address, mode, name
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
	symbols = {}
	missed = []
	index = 0
	with in_p.open(mode='r') as f_i:
		for line in f_i.read().splitlines():
			line = line.strip()
			if (index == 0) and line != ADS_SYM_FILE_HEADER:
				return False
			if len(line) != 0 and not line.startswith('#'):
				address, mode, name = split_and_validate_line(line)
				if name is not None:
					if not symbols.get(name, None):
						symbols[name] = address
					else:
						first_address = symbols[name]
						logging.error(f'Error! Duplicate SYM values:')
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
				return (int(address, 16) + 1) if mode == 'T' else (int(address, 16) + 1)
	return 0x00000000
