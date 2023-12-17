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
	SYMBOLS_LISTING: int = 1
	REGENERATOR: int = 2


# Helpers.


# LibGen working flow.
def start_ep2_libgen_work(mode: Mode, sort: forge.LibrarySort, args: Namespace) -> bool:
	logging.info(f'Start ElfPack v2.0 LibGen utility, mode: {mode.name}.')

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
		s: Path = args.source
		o: Path = args.output
		sort: forge.LibrarySort = self.determine_sort_mode(args)
		if args.all:
			return Mode.REGENERATOR, sort, args
		if forge.check_files_extensions([s], ['sym'], False) and forge.check_files_extensions([o], ['bin'], False):
			return Mode.PHONE_LIBRARY, sort, args
		elif forge.check_files_extensions([s], ['bin'], False) and forge.check_files_extensions([o], ['sym'], False):
			return Mode.SYMBOLS_LISTING, sort, args
		else:
			self.error('unknown --output mode, check output file extension')


def parse_arguments() -> tuple[Mode, forge.LibrarySort, Namespace]:
	hlp: dict[str, str] = {
		'h': 'A libgen utility for ElfPack v1.0 and Motorola phones on P2K platform, 15-Dec-2023',
		's': 'source input library or symbols file',
		'o': 'output resulting file with "*.bin", "*.sa", and "*.sym" extensions',
		'sa': 'sort by addresses',
		'st': 'sort by types',
		'sn': 'sort by names',
		'a': 're-generate all libraries by symbol files in library directory',
		'v': 'verbose output'
	}
	epl: str = """examples:
	python ep2_libgen.py -s library.sym -o library.bin
	python ep2_libgen.py -sn -s library.sym -o library.bin

	python ep2_libgen.py -s library.bin -o library.sym
	python ep2_libgen.py -sn -s library.bin -o library.sym

	python ep2_libgen.py -a
	python ep2_libgen.py -sn -a
	"""
	parser_args: Args = Args(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-s', '--source', required=False, type=forge.at_file, metavar='INPUT', help=hlp['s'])
	parser_args.add_argument('-o', '--output', required=False, type=forge.at_path, metavar='OUTPUT', help=hlp['o'])
	parser_args.add_argument('-sa', '--sort-address', required=False, action='store_true', help=hlp['sa'])
	parser_args.add_argument('-st', '--sort-type', required=False, action='store_true', help=hlp['st'])
	parser_args.add_argument('-sn', '--sort-name', required=False, action='store_true', help=hlp['sn'])
	parser_args.add_argument('-a', '--all', required=False, action='store_true', help=hlp['a'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, sort, args = parse_arguments()

	forge.set_logging_configuration(args.verbose)

	start_ep2_libgen_work(mode, sort, args)


if __name__ == '__main__':
	main()
