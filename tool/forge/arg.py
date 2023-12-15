# forge/arg.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import argparse

from pathlib import Path

from .hexer import hex2int
from .hexer import normalize_hex_string
from .firmware import parse_phone_firmware


def at_fw(firmware_filename: str) -> Path:
	try:
		parse_phone_firmware(firmware_filename)
		return at_file(firmware_filename)
	except ValueError as value_error:
		raise argparse.ArgumentTypeError(value_error)


def at_dir(dirname: str) -> Path:
	path = Path(dirname)
	if not path.exists():
		path.mkdir()
	if not path.exists() or not path.is_dir():
		raise argparse.ArgumentTypeError(f'{dirname} is not directory')
	return path


def at_file(filename: str) -> Path:
	path = Path(filename)
	if not path.is_file() and not path.exists():
		raise argparse.ArgumentTypeError(f'{filename} not found')
	return path


def at_path(filename: str) -> Path:
	return Path(filename)


def at_fpa(filename: str) -> Path:
	at_file(filename)
	path = Path(filename)
	if not path.name.endswith('.fpa'):
		raise argparse.ArgumentTypeError(f'{filename} is not *.fpa patch')
	return path


def at_hex(hex_value: str) -> int:
	try:
		return hex2int(hex_value)
	except ValueError as value_error:
		raise argparse.ArgumentTypeError(value_error)


# HEX DATA STRING, e.g. '0123456789ABCDEF'.
def at_hds(hds: str) -> str:
	hex_data_string = normalize_hex_string(hds)
	if hex_data_string is None:
		raise argparse.ArgumentTypeError(f'wrong hex data string')
	return hex_data_string
