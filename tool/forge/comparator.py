# forge/comparator.py
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

from .types import ElfPack
from .types import ElfPacks
from .types import LibrarySort
from .types import LibraryModel
from .libgen import ep1_libgen_model
from .libgen import ep2_libgen_model
from .symbols import combine_sym_str
from .symbols import split_and_validate_line


def get_library_model(p_sym: Path, elfpack: ElfPack) -> LibraryModel | None:
	if elfpack == ElfPack.EP1:
		functions, model = ep1_libgen_model(p_sym, LibrarySort.NAME)
	elif elfpack == ElfPack.EP2:
		model = ep2_libgen_model(p_sym, LibrarySort.NAME)
	else:
		logging.error(f'Not implemented ElfPack "{elfpack.name}" support.')
		return None
	return model


def sym_cmp_sym(s_sym: Path, c_sym: Path, elfpacks: ElfPacks, names_only: bool) -> bool:
	e1, e2 = elfpacks
	s_model: LibraryModel = get_library_model(s_sym, e1)
	c_model: LibraryModel = get_library_model(c_sym, e2)

	if not s_model:
		logging.error(f'Library model of "{s_sym}" is empty.')
		return False
	if not c_model:
		logging.error(f'Library model of "{c_sym}" is empty.')
		return False

	for c_addr, c_mode, c_name in c_model:
		name_not_found: bool = True
		c_str: str = combine_sym_str(c_addr, c_mode, c_name)
		for s_addr, s_mode, s_name in s_model:
			if s_name == c_name:
				name_not_found = False
				if not names_only:
					s_str: str = combine_sym_str(s_addr, s_mode, s_name)
					if s_str == c_str:
						logging.debug(f'"{c_str}" found in "{s_sym}" file.')
						continue
					if s_mode != c_mode:
						logging.info(f'Modes missmatch:')
					if s_addr != c_addr:
						logging.info(f'Addresses missmatch:')
					logging.info(f'\t"{c_str}" in "{c_sym}" file.')
					logging.info(f'\t"{s_str}" in "{s_sym}" file.')
		if name_not_found:
			logging.info(f'"{c_str}" not found in "{s_sym}" file.')

	return True


def sym_cmp_def(a_sym: Path, a_def: Path, elfpacks: ElfPacks) -> bool:
	e1, e2 = elfpacks
	model: LibraryModel = get_library_model(a_sym, e1)

	if model:
		with a_def.open(mode='r') as f_i:
			found_something: bool = False
			for line in f_i.read().splitlines():
				line: str = line.strip()
				addr, mode, name = split_and_validate_line(line)
				if name:
					line = name
					found: bool = False
					for addr, mode, name in model:
						name: str = name.strip()
						a_str: str = combine_sym_str(addr, mode, name)
						if line == name:
							logging.info(f'"{a_str}" found in "{a_sym}" file.')
							found = True
							found_something = True
							break
						else:
							found = False
					if not found:
						logging.info(f'"{line}" not found in "{a_sym}" file.')
			if not found_something:
				logging.info(f'Nothing found in "{a_sym}" file..')
			return found_something
	else:
		logging.error(f'Library model of "{a_sym}" is empty.')
	return False
