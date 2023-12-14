# forge/invoke.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru developers
"""

import logging
import subprocess


def invoke_external_command(arguments: list[str]) -> int:
	command = ' '.join(arguments)

	logging.info(f'Will execute external command:')
	logging.info(f'{command}')

	result = subprocess.run(arguments).returncode

	logging.info(f'Result of "{command}" is "{result}".')

	return result


def invoke_external_command_res(arguments: list[str]) -> bool:
	return True if (invoke_external_command(arguments) == 0) else False
