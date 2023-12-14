# forge/hexer.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru developers
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
