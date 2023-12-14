# forge/invoke.py

import logging
import subprocess


def invoke_external_command(arguments: list[str]) -> int:
	command = ' '.join(arguments)

	logging.info(f'Will execute external command:')
	logging.info(f'{command}')

	result = subprocess.run(arguments).returncode

	logging.info(f'Result of "{command}" is "{result}".')

	return result
