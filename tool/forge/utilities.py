# forge/utilities.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

from datetime import timedelta


def format_timedelta(td: timedelta) -> str:
	days, seconds = td.days, td.seconds
	hours: int = days * 24 + seconds // 3600
	minutes: int = (seconds % 3600) // 60
	seconds: int = (seconds % 60)
	milliseconds: int = td.microseconds // 1000  # Convert microseconds to milliseconds

	return f'{hours:02d}:{minutes:02d}:{seconds:02d}.{milliseconds:03d}'


def chop_str(res: str, chop: int) -> str:
	length: int = len(res)
	if length <= chop:
		return (res + (' ' * ((chop - 3) - length)) + '...')[:chop]
	else:
		return res[:(chop - 3)] + '...'
