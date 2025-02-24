# forge/arguments.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

import argparse
import logging

from pathlib import Path

from .hexer import hex2int
from .hexer import int2hex
from .utilities import chop_str
from .hexer import normalize_hex_string
from .filesystem import check_files_if_exists
from .filesystem import check_files_extensions
from .firmware import parse_phone_firmware
from .types import ElfPack


# Phone Firmware, e.g. 'E1_R373_G_0E.30.49R'.
def at_pfw(phone_firmware_tuple: str) -> tuple[str, str]:
	try:
		return parse_phone_firmware(phone_firmware_tuple, False)
	except ValueError as value_error:
		raise argparse.ArgumentTypeError(value_error)


# File Firmware, e.g. 'E1_R373_G_0E.30.49R.smg'.
def at_ffw(firmware_filename: str) -> Path:
	try:
		parse_phone_firmware(firmware_filename)
		return at_file(firmware_filename)
	except ValueError as value_error:
		raise argparse.ArgumentTypeError(value_error)


def at_dir(dirname: str) -> Path:
	path: Path = Path(dirname)
	if not path.exists():
		try:
			path.mkdir()
			logging.info(f'Directory "{dirname}" was successfully created.')
		except OSError as error:
			logging.error(f'Cannot create "{dirname}" directory: {error}')
	if not path.is_dir():
		raise argparse.ArgumentTypeError(f'{dirname} is not directory')
	return path


def at_file(filename: str) -> Path:
	path: Path = Path(filename)
	if not check_files_if_exists([path]):
		raise argparse.ArgumentTypeError(f'{filename} not found')
	return path


def at_path(filename: str) -> Path:
	return Path(filename)


def at_fpa(filename: str) -> Path:
	at_file(filename)
	path: Path = Path(filename)
	if not check_files_extensions([path], ['fpa']):
		raise argparse.ArgumentTypeError(f'{filename} is not *.fpa patch')
	return path


def at_hex(hex_value: str) -> int:
	try:
		return hex2int(hex_value)
	except ValueError as value_error:
		raise argparse.ArgumentTypeError(value_error)


# HEX DATA STRING, e.g. '0123456789ABCDEF'.
def at_hds(hds: str) -> str:
	hex_data_string: str = normalize_hex_string(hds)
	if hex_data_string is None:
		raise argparse.ArgumentTypeError(f'wrong hex data string: {chop_str(hds, 32)}')
	return hex_data_string


def at_ep(ep: str) -> ElfPack:
	ep = ep.upper()
	if ep == 'EP1':
		return ElfPack.EP1
	elif ep == 'EP2':
		return ElfPack.EP2
	elif ep == 'EM1':
		return ElfPack.EM1
	elif ep == 'EM2':
		return ElfPack.EM2
	else:
		return ElfPack.UNK


def at_int(argument: str) -> int:
	try:
		value: int = int(argument)
		if value <= 0xFFFFFFFF:
			return value
		raise argparse.ArgumentTypeError(f'int size overflow: {value} ({int2hex(value)}) too long, max is 0xFFFFFFFF')
	except ValueError as value_error:
		raise argparse.ArgumentTypeError(value_error)


def args_dump(arguments: dict[str, any]) -> None:
	for key, value in arguments.items():
		if isinstance(value, int) and not isinstance(value, bool):
			logging.info(f'\t{key}: {int2hex(value) + " " + str(value)}')
		else:
			logging.info(f'\t{key}: {value}')
