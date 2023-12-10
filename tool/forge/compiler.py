# forge/compiler.py

import logging
from pathlib import Path


def generate_source_with_const_chars(header_file: Path, array_dict: dict[str, str]) -> bool:
	try:
		with header_file.open(mode='w', newline='\r\n') as f_o:
			for key, value in array_dict.items():
				length = len(value) + 1  # '\0'
				template = f'const char {key}[{length}]\t= "{value}";'
				logging.debug(template)
				f_o.write(template)
				f_o.write('\n')
		return True
	except FileNotFoundError as error:
		logging.error(error)
		return False
