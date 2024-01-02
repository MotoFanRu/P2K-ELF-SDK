# forge/hexer.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""


def hex2int(hex_value: str, size: int = 8) -> int:
	if not hex_value.startswith('0x'):
		raise ValueError(f'value "{hex_value}" should starts with a "0x" prefix')
	if len(hex_value) != (size + 2):
		raise ValueError(f'value "{hex_value}" should be in the "{size} + 2" format like "0x000012FF", "0x12FF" digit')
	try:
		return int(hex_value, 16)
	except ValueError:
		raise ValueError(f'value "{hex_value}" is not a valid hexadecimal value')


def hex2int_r(hex_value: str) -> int:
	if hex_value.startswith('0x'):
		raise ValueError(f'value "{hex_value}" should starts with no "0x" prefix')
	if len(hex_value) != 8:
		raise ValueError(f'value "{hex_value}" should be in the "8" format like "000012FF" hex digit')
	try:
		return int(hex_value, 16)
	except ValueError:
		raise ValueError(f'value "{hex_value}" is not a valid hexadecimal value')


def int2hex(int_value: int, size: int = 8) -> str:
	return f'0x{int_value:0{size}X}'


def int2hex_r(int_value: int) -> str:
	return f'{int_value:08X}'


def arrange16(value: int) -> int:
	return (value & (-16)) + 16


def is_hex_string(hex_string: str) -> bool:
	hex_digits: set[str] = set('0123456789abcdefABCDEF')
	return all(char in hex_digits for char in hex_string)


def normalize_hex_string(hex_string: str) -> str | None:
	hex_string: str = hex_string.strip().upper()
	return hex_string if is_hex_string(hex_string) else None


def normalize_hex_address(hex_address: str, raw: bool) -> str | None:
	hex_address: str = hex_address.strip().upper()
	if len(hex_address) <= 8:
		value: int = int(hex_address, 16)
		if raw:
			return int2hex_r(value)
		return int2hex(value)
	return None


def hex2hex(hex_value: str, size: int = 8) -> str:
	max_values: dict[int, int] = {
		2: 0xFF,
		4: 0xFFFF,
		6: 0xFFFFFF,
		8: 0xFFFFFFFF
	}

	if size not in max_values:
		raise ValueError(f'size "{size}" not supported, please use 2, 4, 6, or 8')

	try:
		value: int = int(hex_value, 16)
		if value <= max_values[size]:
			return int2hex(value, size)
	except ValueError:
		raise ValueError(f'value "{hex_value}" is not a HEX number')

	raise ValueError(f'value "{hex_value}" exceeds the maximum for size "{size}"')


def str2hex(str_value: str) -> str:
	return str_value.encode('ascii').hex().upper()
