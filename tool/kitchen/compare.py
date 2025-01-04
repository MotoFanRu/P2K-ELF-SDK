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
	e_reverse: forge.ElfPack = args.elfpack_src
	args.elfpack_src = args.elfpack_cmp
	args.elfpack_cmp = e_reverse
	return args


# Comparator working flow.
def start_comparator_work(mode: Mode, args: Namespace) -> bool:
	logging.info(f'Start Comparator utility, mode: "{mode.name}".')
	if mode == Mode.SYM_TO_SYM:
		if args.swap:
			args = swap_reverse_arguments(args)
		return forge.log_result(
			forge.sym_cmp_sym(args.source, args.compare, (args.elfpack_src, args.elfpack_cmp), args.names)
		)
	elif mode == Mode.SYM_TO_DEF:
		return forge.log_result(forge.sym_cmp_def(args.source, args.compare, (args.elfpack_src, args.elfpack_cmp)))
	elif mode == Mode.PAT_TO_PAT:
		if args.swap:
			args = swap_reverse_arguments(args)
		return forge.log_result(forge.pat_cmp_pat(args.source, args.compare, args.names))
	return False


# Arguments parsing routines.
class Args(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')

	def parse_check_arguments(self) -> tuple[Mode, Namespace]:
		args: Namespace = self.parse_args()
		if not args.elfpack_src:
			args.elfpack_src = forge.ElfPack.EP1
		if not args.elfpack_cmp:
			args.elfpack_cmp = forge.ElfPack.EP1

		s: Path = args.source
		c: Path = args.compare

		s_sym = forge.check_files_extensions([s], ['sym'], False)
		c_def = forge.check_files_extensions([c], ['def'], False)
		c_sym = forge.check_files_extensions([c], ['sym'], False)
		s_pat = forge.check_files_extensions([s], ['pts'], False)
		c_pat = forge.check_files_extensions([c], ['pts'], False)

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
		'es': 'ElfPack version of source file',
		'c': 'compare file',
		'ec': 'ElfPack version of compare file',
		'r': 'reverse and swap source and compare arguments',
		'n': 'compare names only',
		'v': 'verbose output'
	}
	epl: str = """examples:
	# Compare symbols files among themselves (+swap/reverse arguments, names only).
	python compare.py -s elfloader_1.sym -c elfloader_2.sym
	python compare.py -s elfloader_1.sym -c library_2.sym -ec EP2
	python compare.py -s library_1.sym -es EP2 -c library_2.sym -ec EP2
	python compare.py -s library_1.sym -es EP2 -c elfloader_2.sym
	python compare.py -s elfloader_1.sym -c library_2.sym -ec EP2 -r
	python compare.py -s elfloader_1.sym -c library_2.sym -ec EP2 -n

	# Compare symbols file with name definition file.
	python compare.py -s elfloader.sym -c ../../ep1/def/ElfLoaderAPI1.def
	python compare.py -s elfloader.sym -c ../../ep2/def/ElfLoaderAPI2.def -ec EP2
	python compare.py -s library.sym -es EP2 -c ep2/def/EntriesNames.def -ec EP2

	# Compare patterns files among themselves (+swap/reverse arguments, names only).
	python compare.py -s patterns_1.pts -c patterns_2.pts
	python compare.py -s patterns_1.pts -c patterns_2.pts -r
	python compare.py -s patterns_1.pts -c patterns_2.pts -n
	"""
	parser_args: Args = Args(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-s', '--source', required=True, type=forge.at_file, metavar='INPUT', help=hlp['s'])
	parser_args.add_argument('-c', '--compare', required=True, type=forge.at_file, metavar='COMPARE', help=hlp['c'])
	parser_args.add_argument('-es', '--elfpack-src', required=False, type=forge.at_ep, metavar='ELFPACK', help=hlp['es'])
	parser_args.add_argument('-ec', '--elfpack-cmp', required=False, type=forge.at_ep, metavar='ELFPACK', help=hlp['ec'])
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
