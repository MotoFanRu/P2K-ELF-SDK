# forge/file.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import logging

from pathlib import Path


def move_file(from_p: Path, to_path: Path) -> Path:
	return from_p.replace(to_path)


def delete_all_files_in_directory(directory: Path) -> None:
	objects = directory.iterdir()
	files_to_clean = []
	for obj in objects:
		if obj.is_file():
			files_to_clean.append(obj)
	if len(files_to_clean) > 0:
		logging.info(f'Clean all files in "{directory}" directory.')
		for file_path in files_to_clean:
			if file_path.is_file():
				logging.info(f'\tDelete "{file_path}" file.')
				file_path.unlink()
		logging.info(f'')
