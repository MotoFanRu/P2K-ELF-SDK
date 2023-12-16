# forge/filesystem.py
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


def delete_all_files_in_directory(directory: Path) -> bool:
	files_to_clean: list[Path] = []
	for object_path in directory.iterdir():
		if object_path.is_file():
			files_to_clean.append(object_path)
	if len(files_to_clean) > 0:
		logging.info(f'Clean all files in "{directory}" directory.')
		for file_path in files_to_clean:
			logging.info(f'\tDelete "{file_path}" file.')
			file_path.unlink()
		logging.info(f'')
		return True
	return False


def check_directories_if_exists(p_dirs: list[Path]) -> bool:
	for dir_path in p_dirs:
		if dir_path is not None:
			if not dir_path.is_dir():
				logging.error(f'Directory "{dir_path}" is not exist or not a directory.')
				return False
		else:
			return False
	return True


def check_files_if_exists(p_files: list[Path]) -> bool:
	for file_path in p_files:
		if file_path is not None:
			if not file_path.is_file():
				logging.error(f'File "{file_path}" is not exist or not a file.')
				return False
		else:
			return False
	return True


def check_files_extensions(p_files: list[Path], extensions: list[str]) -> bool:
	checked_files: dict[Path, bool] = {}
	for file_path in p_files:
		checked_files[file_path] = False
		for extension in extensions:
			if file_path.name.endswith('.' + extension):
				checked_files[file_path] = True
	for file_path, checked in checked_files.items():
		if not checked:
			logging.error(f'File "{file_path}" does not have "[{extensions}]" extensions.')
			return False
	return True
