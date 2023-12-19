# forge/utilities.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

import logging

from pathlib import Path
from datetime import datetime
from datetime import timedelta

from .filesystem import check_files_if_exists


def get_current_datetime_formatted() -> str:
	dt_now = datetime.now()
	return dt_now.strftime('%d-%b-%Y %H:%M:%S')


def format_timedelta(td: timedelta) -> str:
	days, seconds = td.days, td.seconds
	hours: int = days * 24 + seconds // 3600
	minutes: int = (seconds % 3600) // 60
	seconds: int = (seconds % 60)
	milliseconds: int = td.microseconds // 1000  # Convert microseconds to milliseconds

	return f'{hours:02d}:{minutes:02d}:{seconds:02d}.{milliseconds:03d}'


def chop_str(res: str, chop: int = 24, arrange: bool = False) -> str:
	length: int = len(res)
	if length <= chop:
		return (res + (' ' * ((chop - 3) - length)) + '...')[:chop] if arrange else res
	else:
		return res[:(chop - 3)] + '...'


def log_result(result: bool) -> bool:
	logging.info(f'Done.' if result else f'Fail.')
	return result


def dump_text_file_to_debug_log(text_file: Path, strip_lines: bool = True) -> None:
	if check_files_if_exists([text_file]):
		with text_file.open(mode='r') as f_i:
			for line in f_i.readlines():
				if strip_lines:
					line = line.strip()
				else:
					line = line.replace('\n', '')
				logging.debug(f'{line}')


def set_logging_configuration(verbose: bool) -> None:
	log_format: str = '%(asctime)s %(levelname)s: %(message)s'
	if verbose:
		log_format = '%(asctime)s %(levelname)s [%(funcName)s]: %(message)s'

	logging.basicConfig(
		level=logging.DEBUG if verbose else logging.INFO,
		format=log_format,
		datefmt='%d-%b-%Y %H:%M:%S'
	)
