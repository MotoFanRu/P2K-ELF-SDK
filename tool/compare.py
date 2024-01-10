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
	PAT_TO_PAT: int = 2


# Helpers.
def swap_reverse_arguments(args: Namespace) -> Namespace:
	p_reverse: Path = args.source
	args.source = args.compare
	args.compare = p_reverse
	e_reverse: forge.ElfPack = args.elfpack1
	args.elfpack1 = args.elfpack2
	args.elfpack2 = e_reverse
	return args


# Comparator working flow.
def start_comparator_work(mode: Mode, args: Namespace) -> bool:
	logging.info(f'Start Comparator utility, mode: "{mode.name}".')
	if mode == Mode.SYM_TO_SYM:
		if args.swap:
			args = swap_reverse_arguments(args)
		return forge.log_result(
			forge.sym_cmp_sym(args.source, args.compare, (args.elfpack1, args.elfpack2), args.names)
		)
	elif mode == Mode.SYM_TO_DEF:
		return forge.log_result(forge.sym_cmp_def(args.source, args.compare, (args.elfpack1, args.elfpack2)))
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
		s_pat = forge.check_files_extensions([s], ['pat'], False)
		c_pat = forge.check_files_extensions([c], ['pat'], False)

		if s_sym and c_def:
			return Mode.SYM_TO_DEF, args
		elif s_sym and c_sym:
			return Mode.SYM_TO_SYM, args
		elif s_pat and c_pat:
			return Mode.PAT_TO_PAT, args

		self.error('all arguments are empty')


def parse_arguments() -> tuple[Mode, Namespace]:
	hlp: dict[str, str] = {
		'h': 'A comparator utility for various ElfPacks and Motorola phones on P2K platform, 15-Dec-2023',
		's': 'source file',
		'e1': 'ElfPack version of source file',
		'c': 'compare file',
		'e2': 'ElfPack version of compare file',
		'r': 'reverse and swap source and compare arguments',
		'n': 'compare names only',
		'v': 'verbose output'
	}
	epl: str = """examples:
	# Compare symbols files among themselves (+swap/reverse arguments, names only).
	python compare.py -s elfloader_1.sym -e1 EP1 -c elfloader_2.sym -e2 EP1
	python compare.py -s elfloader_1.sym -e1 EP1 -c library_2.sym -e2 EP2
	python compare.py -s library_1.sym -e1 EP2 -c library_2.sym -e2 EP2
	python compare.py -s library_1.sym -e1 EP2 -c elfloader_2.sym -e2 EP1
	python compare.py -s elfloader_1.sym -e1 EP1 -c library_2.sym -e2 EP2 -r
	python compare.py -s elfloader_1.sym -e1 EP1 -c library_2.sym -e2 EP2 -n

	# Compare symbols file with name definition file.
	python compare.py -s elfloader.sym -e1 EP1 -c ep2/def/ElfLoaderAPI1.def -e2 EP1
	python compare.py -s elfloader.sym -e1 EP1 -c ep2/def/ElfLoaderAPI2.def -e2 EP2
	python compare.py -s library.sym -e1 EP2 -c ep2/def/EntriesNames.def -e2 EP2
	"""
	parser_args: Args = Args(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-s', '--source', required=True, type=forge.at_file, metavar='INPUT', help=hlp['s'])
	parser_args.add_argument('-c', '--compare', required=True, type=forge.at_file, metavar='COMPARE', help=hlp['c'])
	parser_args.add_argument('-e1', '--elfpack1', required=True, type=forge.at_ep, metavar='ELFPACK', help=hlp['e1'])
	parser_args.add_argument('-e2', '--elfpack2', required=True, type=forge.at_ep, metavar='ELFPACK', help=hlp['e2'])
	parser_args.add_argument('-r', '--swap', required=False, action='store_true', help=hlp['r'])
	parser_args.add_argument('-n', '--names', required=False, action='store_true', help=hlp['n'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, args = parse_arguments()

	forge.set_logging_configuration(args.verbose)

	start_comparator_work(mode, args)


if __name__ == '__main__':
	main()
