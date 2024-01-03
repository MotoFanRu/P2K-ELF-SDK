# forge/invoker.py
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
import subprocess

from pathlib import Path

from .filesystem import check_files_if_exists


def invoke_external_system_command(arguments: list[str]) -> int:
	command: str = ' '.join(arguments)

	logging.info('Will execute external system command:')
	logging.info(f'{command}')
	logging.info('')

	result: int = subprocess.run(arguments).returncode

	logging.info('Result:')
	logging.info(f'{result}')

	return result


def invoke_custom_arguments(custom_flags: list[str] | None = None) -> list[str]:
	if custom_flags is None:
		custom_flags: list[str] = []
	return custom_flags


def invoke_external_command_res(p_in: list[Path], arguments: list[str]) -> bool:
	if check_files_if_exists(p_in):
		return invoke_external_system_command(arguments) == 0
	return False
