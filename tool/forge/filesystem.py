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
import tempfile

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


def normalize_extension(extension: str) -> str:
	if not extension.startswith('.'):
		return '.' + extension
	return extension


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


def check_files_extensions(p_files: list[Path], extensions: list[str], log_output: bool = True) -> bool:
	checked_files: dict[Path, bool] = {}
	for file_path in p_files:
		checked_files[file_path] = False
		for extension in extensions:
			if file_path.name.endswith(normalize_extension(extension)):
				checked_files[file_path] = True
	for file_path, checked in checked_files.items():
		if not checked:
			if log_output:
				logging.error(f'File "{file_path}" does not have "[{extensions}]" extensions.')
			return False
	return True


def get_temporary_directory_path() -> Path:
	return Path(tempfile.gettempdir())


def create_temporary_file_with_extension(extension: str) -> Path:
	extension = normalize_extension(extension)
	with tempfile.NamedTemporaryFile(suffix=extension, delete=False) as temp_file:
		return Path(temp_file.name)


def delete_file(file_path: Path) -> bool:
	if check_files_if_exists([file_path]):
		try:
			file_path.unlink()
			logging.info(f'File "{file_path}" was deleted.')
			return True
		except OSError as error:
			logging.error(f'Cannot delete "{file_path}", error: {error}')
	return False
