#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A libgen utility for ElfPack v1.0 and Motorola phones on P2K platform.

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
	ASSEMBLER_LISTING: int = 1
	OBJECT_LIBRARY: int = 2
	STATIC_LIBRARY: int = 3


# Helpers.
def sym2obj(library_model: forge.LibraryModel, p_i: Path, p_o: Path) -> bool:
	temp_asm_file = forge.get_temporary_directory_path() / 'Lib.asm'
	logging.info(f'Will create "{temp_asm_file}" assembly listing from "{p_i}" symbols file.')
	if forge.log_result(forge.ep1_libgen_asm(temp_asm_file, library_model)):
		logging.info(f'Will create "{p_o}" object library from "{temp_asm_file}" assembly listing.')
		result = forge.log_result(forge.ep1_ads_armasm(temp_asm_file, p_o))
		forge.delete_file(temp_asm_file)
		return result
	return False


def sym2lib(library_model: forge.LibraryModel, p_i: Path, p_o: Path) -> bool:
	temp_object_library_file = forge.get_temporary_directory_path() / 'Lib.o'
	if sym2obj(library_model, p_i, temp_object_library_file):
		logging.info(f'Will create "{p_o}" static library from "{temp_object_library_file}" object file.')
		result = forge.ep1_ads_armar([temp_object_library_file], p_o)
		forge.delete_file(temp_object_library_file)
		return result
	return False


# LibGen working flow.
def start_ep1_libgen_work(mode: Mode, args: Namespace) -> bool:
	logging.info(f'Start ElfPack v1.0 LibGen utility, mode: {mode.name}.')

	library_model: forge.LibraryModel = []
	functions: str = forge.ep1_libgen_model(args.symbols, library_model)

	if mode == Mode.PHONE_LIBRARY:
		logging.info(f'Will create "{args.output}" library from "{args.symbols}" symbols file.')
		return forge.log_result(forge.ep1_libgen_library(args.output, library_model, functions))
	elif mode == Mode.ASSEMBLER_LISTING:
		logging.info(f'Will create "{args.output}" assembly listing from "{args.symbols}" symbols file.')
		return forge.log_result(forge.ep1_libgen_asm(args.output, library_model))
	elif mode == Mode.OBJECT_LIBRARY:
		return sym2obj(library_model, args.symbols, args.output)
	elif mode == Mode.STATIC_LIBRARY:
		return sym2lib(library_model, args.symbols, args.output)
	else:
		logging.error(f'Unknown mode: {mode.name}')

	return False


# Arguments parsing routines.
class Args(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')

	def parse_check_arguments(self) -> tuple[Mode, Namespace]:
		args: Namespace = self.parse_args()
		if forge.check_files_extensions([args.output], ['lib'], False):
			return Mode.PHONE_LIBRARY, args
		elif forge.check_files_extensions([args.output], ['asm'], False):
			return Mode.ASSEMBLER_LISTING, args
		elif forge.check_files_extensions([args.output], ['o'], False):
			return Mode.OBJECT_LIBRARY, args
		elif forge.check_files_extensions([args.output], ['a'], False):
			return Mode.STATIC_LIBRARY, args
		else:
			self.error('unknown --output mode, check output file extension')


def parse_arguments() -> tuple[Mode, Namespace]:
	hlp: dict[str, str] = {
		'h': 'A libgen utility for ElfPack v1.0 and Motorola phones on P2K platform, 15-Dec-2023',
		's': 'symbols file with functions and other addresses',
		'o': 'output resulting file with "*.lib", "*.a", ".asm", and "*.o" extensions',
		'v': 'verbose output'
	}
	epl: str = """examples:
	python ep1_libgen.py -s Lib.sym -o elfloader.lib
	python ep1_libgen.py -s Lib.sym -o libstd.a
	python ep1_libgen.py -s Lib.sym -o Lib.asm
	python ep1_libgen.py -s Lib.sym -o Lib.o
	"""
	parser_args: Args = Args(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-s', '--symbols', required=True, type=forge.at_sym, metavar='SYMBOLS', help=hlp['s'])
	parser_args.add_argument('-o', '--output', required=True, type=forge.at_path, metavar='OUTPUT', help=hlp['o'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, args = parse_arguments()

	logging.basicConfig(
		level=logging.DEBUG if args.verbose else logging.INFO,
		format='%(asctime)s %(levelname)s: %(message)s',
		datefmt='%d-%b-%Y %H:%M:%S'
	)

	start_ep1_libgen_work(mode, args)
	if mode == Mode.ASSEMBLER_LISTING:
		logging.debug(f'')
		forge.dump_text_file_to_debug_log(args.output, strip_lines=False)


if __name__ == '__main__':
	main()
