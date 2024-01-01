#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A Comparator utility for various ElfPacks and Motorola phones on P2K platform.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

import sys
import logging
import argparse

import forge

from enum import Enum
from pathlib import Path
from argparse import Namespace


class Mode(Enum):
	SYM_TO_SYM: int = 0
	SYM_TO_DEF: int = 1


# Comparators.
def cmp_sym_def(a_sym: Path, a_def: Path, elfpacks: forge.ElfPacks) -> bool:
	e1, e2 = elfpacks
	model: forge.LibraryModel = []

	if e1 == forge.ElfPack.EP1:
		functions, model = forge.ep1_libgen_model(a_sym, forge.LibrarySort.NAME)
	elif e1 == forge.ElfPack.EP2:
		model = forge.ep2_libgen_model(a_sym, forge.LibrarySort.NAME)

	if model:
		with a_def.open(mode='r') as f_i:
			found_something: bool = False
			for line in f_i.read().splitlines():
				line: str = line.strip()
				addr, mode, name = forge.split_and_validate_line(line)
				if name:
					line = name
				found: bool = False
				for addr, mode, name in model:
					name: str = name.strip()
					if line == name:
						logging.info(f'Found: "{line}" as "{addr} {mode} {name}" in "{a_sym}" file.')
						found = True
						found_something = True
						break
					else:
						found = False
				if not found:
					logging.info(f'Not Found: "{line}".')
			if not found_something:
				logging.info(f'Nothing found.')
			return found_something
	return False


# Comparator working flow.
def start_comparator_work(mode: Mode, args: Namespace) -> bool:
	logging.info(f'Start Comparator utility, mode: "{mode.name}".')
	if mode == Mode.SYM_TO_DEF:
		return forge.log_result(cmp_sym_def(args.source, args.compare, (args.ep1, args.ep2)))
	elif mode == Mode.SYM_TO_SYM:
		pass
	return False


# Arguments parsing routines.
class Args(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')

	def parse_check_arguments(self) -> tuple[Mode, Namespace]:
		args: Namespace = self.parse_args()

		s: Path = args.source
		c: Path = args.compare

		s_sym = forge.check_files_extensions([s], ['sym'], False)
		c_def = forge.check_files_extensions([c], ['def'], False)
		c_sym = forge.check_files_extensions([c], ['sym'], False)

		if s_sym and c_def:
			return Mode.SYM_TO_DEF, args
		elif s_sym and c_sym:
			return Mode.SYM_TO_SYM, args

		self.error('all arguments are empty')


def parse_arguments() -> tuple[Mode, Namespace]:
	hlp: dict[str, str] = {
		'h': 'A comparator utility for various ElfPacks and Motorola phones on P2K platform, 15-Dec-2023',
		's': 'source file',
		'e1': 'ElfPack version of source file',
		'c': 'compare file',
		'e2': 'ElfPack version of compare file',
		'v': 'verbose output'
	}
	epl: str = """examples:
	# Compare symbols files among themselves.
	python compare.py -s library_1.sym -e1 EP1 -c library_2.sym -e2 EP1

	# Compare symbols file with name definition file.
	python compare.py -s elfloader.sym -e1 EP1 -c ep2/def/ElfLoaderAPI.def -e2 EP2
	python compare.py -s library.sym -e1 EP2 -c ep2/def/EntriesNames.def -e2 EP2
	"""
	parser_args: Args = Args(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-s', '--source', required=True, type=forge.at_file, metavar='INPUT', help=hlp['s'])
	parser_args.add_argument('-c', '--compare', required=True, type=forge.at_file, metavar='COMPARE', help=hlp['c'])
	parser_args.add_argument('-e1', '--ep1', required=True, type=forge.at_ep, metavar='ELFPACK', help=hlp['e1'])
	parser_args.add_argument('-e2', '--ep2', required=True, type=forge.at_ep, metavar='ELFPACK', help=hlp['e2'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, args = parse_arguments()

	forge.set_logging_configuration(args.verbose)

	start_comparator_work(mode, args)


if __name__ == '__main__':
	main()
