# forge/hexer.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""


def hex2int(hex_value: str) -> int:
	if not hex_value.startswith('0x'):
		raise ValueError(f'value "{hex_value}" should starts with a "0x" prefix')
	if len(hex_value) != (8 + 2):  # 0x12345678
		raise ValueError(f'value "{hex_value}" should be in the "8 + 2" format like "0x12345678" hex digit')
	try:
		return int(hex_value, 16)
	except ValueError:
		raise ValueError(f'value "{hex_value}" is not a valid hexadecimal value')


def int2hex(int_value: int) -> str:
	return f'0x{int_value:08X}'


def int2hex_r(int_value: int) -> str:
	return f'{int_value:08X}'


def arrange16(value: int) -> int:
	return (value & -16) + 16


def is_hex_string(hex_string: str) -> bool:
	hex_digits = set('0123456789abcdefABCDEF')
	return all(char in hex_digits for char in hex_string)


def normalize_hex_string(hex_string: str) -> str | None:
	hex_string = hex_string.strip().upper()
	return hex_string if is_hex_string(hex_string) else None
