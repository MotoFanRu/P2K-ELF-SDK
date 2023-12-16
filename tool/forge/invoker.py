# forge/invoker.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import logging
import subprocess

from pathlib import Path


def invoke_external_system_command(arguments: list[str]) -> int:
	command: str = ' '.join(arguments)

	logging.info(f'Will execute external system command:')
	logging.info(f'{command}')

	result: int = subprocess.run(arguments).returncode

	logging.info(f'Executed command:')
	logging.info(f'{command}')
	logging.info(f'Result:')
	logging.info(f'{result}')

	return result


def invoke_custom_arguments(custom_flags: list[str] | None = None) -> list[str]:
	if custom_flags is None:
		custom_flags: list[str] = []
	return custom_flags


def invoke_external_command_res(p_in: list[Path], arguments: list[str]) -> bool:
	for file_path in p_in:
		if not file_path.is_file():
			logging.error(f'File "{file_path}" is not exist or not a file.')
			return False
	return invoke_external_system_command(arguments) == 0