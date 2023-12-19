#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A libgen utility for ElfPack v2.0 and Motorola phones on P2K platform.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import sys
import logging
import argparse

import forge

from enum import Enum
from pathlib import Path
from argparse import Namespace


class Mode(Enum):
	PHONE_LIBRARY: int = 0
	SDK_STUB_LIBRARY: int = 1
	SYMBOLS_LISTING: int = 2
	NAME_DEFINES: int = 3
	REGENERATOR: int = 4


# LibGen working flow.
def start_ep2_libgen_work(mode: Mode, sort: forge.LibrarySort, args: Namespace) -> bool:
	logging.info(f'Start ElfPack v2.0 LibGen utility, mode: "{mode.name}", sort: "{sort.name}".')
	if (mode == Mode.PHONE_LIBRARY) or (mode == Mode.SDK_STUB_LIBRARY):
		logging.info(f'Will create "{args.output}" library from "{args.source}" symbols file.')
		phone, firmware = args.phone_fw
		return forge.log_result(forge.ep2_libgen_library(args.source, sort, phone, firmware, args.output))
	elif mode == Mode.SYMBOLS_LISTING:
		logging.info(f'Will create "{args.output}" symbols file "{args.source}" library.')
		resolve_names: bool = not args.no_resolve_names
		return forge.log_result(forge.ep2_libgen_symbols(args.source, args.output, args.phone, sort, resolve_names))
	elif mode == Mode.NAME_DEFINES:
		logging.info(f'Will create "{args.defines}" file from SDK and "{forge.P2K_DIR_LIB}" directory.')
		return forge.log_result(forge.ep2_libgen_generate_names_defines(sort, args.defines))
	elif mode == Mode.REGENERATOR:
		logging.info(f'Will regenerate all libraries from symbols files in "{forge.P2K_DIR_LIB}" directory.')
		return forge.log_result(forge.ep2_libgen_regenerator(sort))
	return False


# Arguments parsing routines.
class Args(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')

	@staticmethod
	def determine_sort_mode(args: Namespace) -> forge.LibrarySort:
		if args.sort_name:
			return forge.LibrarySort.NAME
		elif args.sort_address:
			return forge.LibrarySort.ADDR
		elif args.sort_type:
			return forge.LibrarySort.MODE
		return forge.LibrarySort.NONE

	def parse_check_arguments(self) -> tuple[Mode, forge.LibrarySort, Namespace]:
		args: Namespace = self.parse_args()
		sort: forge.LibrarySort = self.determine_sort_mode(args)
		if args.all:
			return Mode.REGENERATOR, sort, args
		if args.defines is not None:
			return Mode.NAME_DEFINES, sort, args
		s: Path = args.source
		o: Path = args.output
		pfw: tuple[str, str] = args.phone_fw
		if s is None:
			self.error('source argument is empty')
		if o is None:
			self.error('output argument is empty')
		out_bin = forge.check_files_extensions([o], ['bin'], False)
		out_sym = forge.check_files_extensions([o], ['sym'], False)
		out_sa = forge.check_files_extensions([o], ['sa'], False)
		if forge.check_files_extensions([s], ['sym'], False) and (out_bin or out_sa):
			if pfw is None:
				self.error('phone_fw argument is empty')
			if out_bin:
				return Mode.PHONE_LIBRARY, sort, args
			elif out_sa:
				return Mode.SDK_STUB_LIBRARY, sort, args
		elif forge.check_files_extensions([s], ['bin'], False) and out_sym:
			if not args.phone:
				self.error('phone argument is empty')
			return Mode.SYMBOLS_LISTING, sort, args
		self.error('all arguments are empty')


def parse_arguments() -> tuple[Mode, forge.LibrarySort, Namespace]:
	hlp: dict[str, str] = {
		'h': 'A libgen utility for ElfPack v1.0 and Motorola phones on P2K platform, 15-Dec-2023',
		's': 'source input library or symbols file',
		'o': 'output resulting file with "*.bin", "*.sa", and "*.sym" extensions',
		'p': 'phone model, e.g. "E1"',
		'pf': 'phone and firmware, e.g. "E1_R373_G_0E.30.49R"',
		'sa': 'sort by addresses',
		'st': 'sort by types',
		'sn': 'sort by names',
		'rn': 'do not resolve names',
		'd': 'generate library defines file for resolve "D" (DATA) and "C" (CONST) names',
		'a': 're-generate all libraries by symbol files in library directory',
		'v': 'verbose output'
	}
	epl: str = """examples:
	python ep2_libgen.py -s library.sym -pf 'E1_R373_G_0E.30.49R' -o library.bin
	python ep2_libgen.py -sn -s library.sym -pf 'E1_R373_G_0E.30.49R' -o library.bin

	python ep2_libgen.py -s library.sym -pf 'E1_R373_G_0E.30.49R' -o library.sa
	python ep2_libgen.py -sn -s library.sym -pf 'E1_R373_G_0E.30.49R' -o library.sa

	python ep2_libgen.py -s library.bin -p 'E1' -o library.sym
	python ep2_libgen.py -sn -s library.bin -p 'E1' -o library.sym
	python ep2_libgen.py -sn -rn -s library.bin -p 'E1' -o library.sym

	python ep2_libgen.py -d names.def
	python ep2_libgen.py -sn -d names.def

	python ep2_libgen.py -a
	python ep2_libgen.py -sn -a
	"""
	parser_args: Args = Args(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-s', '--source', required=False, type=forge.at_file, metavar='INPUT', help=hlp['s'])
	parser_args.add_argument('-o', '--output', required=False, type=forge.at_path, metavar='OUTPUT', help=hlp['o'])
	parser_args.add_argument('-p', '--phone', required=False, type=str, metavar='PHONE', help=hlp['p'])
	parser_args.add_argument('-pf', '--phone-fw', required=False, type=forge.at_pfw, metavar='PHONE_FW', help=hlp['pf'])
	parser_args.add_argument('-sa', '--sort-address', required=False, action='store_true', help=hlp['sa'])
	parser_args.add_argument('-st', '--sort-type', required=False, action='store_true', help=hlp['st'])
	parser_args.add_argument('-sn', '--sort-name', required=False, action='store_true', help=hlp['sn'])
	parser_args.add_argument('-rn', '--no-resolve-names', required=False, action='store_true', help=hlp['rn'])
	parser_args.add_argument('-d', '--defines', required=False, type=forge.at_path, metavar='DEFINES', help=hlp['d'])
	parser_args.add_argument('-a', '--all', required=False, action='store_true', help=hlp['a'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, sort, args = parse_arguments()

	forge.set_logging_configuration(args.verbose)

	start_ep2_libgen_work(mode, sort, args)


if __name__ == '__main__':
	main()
