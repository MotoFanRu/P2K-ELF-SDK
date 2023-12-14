# forge/arg.py

import argparse
from pathlib import Path

from .firmware import parse_phone_firmware
from .hexer import hex2int


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


def at_hex(hex_value: str) -> int:
	try:
		return hex2int(hex_value)
	except ValueError as value_error:
		raise argparse.ArgumentTypeError(value_error)
