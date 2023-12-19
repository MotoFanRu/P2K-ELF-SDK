#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A Library Generator utility for ElfPack v1.0 and Motorola phones on P2K platform.

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
	PHONE_LIBRARY: int = 0
	ASSEMBLER_LISTING: int = 1
	OBJECT_LIBRARY: int = 2
	STATIC_LIBRARY: int = 3
	SYMBOLS_LISTING: int = 4


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
def start_ep1_libgen_work(mode: Mode, sort: forge.LibrarySort, args: Namespace) -> bool:
	logging.info(f'Start ElfPack v1.0 LibGen utility, mode: {mode.name}.')

	if mode == Mode.SYMBOLS_LISTING:
		phone, firmware = forge.parse_phone_firmware(args.phone_fw, False)
		logging.info(f'Will create "{args.output}" symbols file from "{args.source}" library to "{phone}_{firmware}".')
		return forge.log_result(forge.ep1_libgen_symbols(args.source, args.output, sort, phone, firmware))
	else:
		functions, library_model = forge.ep1_libgen_model(args.source, sort)

		if mode == Mode.PHONE_LIBRARY:
			logging.info(f'Will create "{args.output}" library from "{args.source}" symbols file.')
			return forge.log_result(forge.ep1_libgen_library(args.output, library_model, functions))
		elif mode == Mode.ASSEMBLER_LISTING:
			logging.info(f'Will create "{args.output}" assembly listing from "{args.source}" symbols file.')
			return forge.log_result(forge.ep1_libgen_asm(args.output, library_model))
		elif mode == Mode.OBJECT_LIBRARY:
			return sym2obj(library_model, args.source, args.output)
		elif mode == Mode.STATIC_LIBRARY:
			return sym2lib(library_model, args.source, args.output)
		else:
			logging.error(f'Unknown mode: {mode.name}')

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
		if forge.check_files_extensions([s], ['sym'], False) and forge.check_files_extensions([o], ['lib'], False):
			return Mode.PHONE_LIBRARY, sort, args
		elif forge.check_files_extensions([s], ['sym'], False) and forge.check_files_extensions([o], ['asm'], False):
			return Mode.ASSEMBLER_LISTING, sort, args
		elif forge.check_files_extensions([s], ['sym'], False) and forge.check_files_extensions([o], ['o'], False):
			return Mode.OBJECT_LIBRARY, sort, args
		elif forge.check_files_extensions([s], ['sym'], False) and forge.check_files_extensions([o], ['a'], False):
			return Mode.STATIC_LIBRARY, sort, args
		elif forge.check_files_extensions([s], ['lib'], False) and forge.check_files_extensions([o], ['sym'], False):
			if args.phone_fw is None:
				self.error('phone_fw argument is empty')
			return Mode.SYMBOLS_LISTING, sort, args
		else:
			self.error('unknown --output mode, check output file extension')


def parse_arguments() -> tuple[Mode, forge.LibrarySort, Namespace]:
	hlp: dict[str, str] = {
		'h': 'A Library Generator utility for ElfPack v1.0 and Motorola phones on P2K platform, 15-Dec-2023',
		's': 'source input library or symbols file',
		'o': 'output resulting file with "*.lib", "*.a", ".asm", "*.o", and "*.sym" extensions',
		'pf': 'phone and firmware, e.g. "E1_R373_G_0E.30.49R"',
		'sa': 'sort by addresses',
		'st': 'sort by types',
		'sn': 'sort by names',
		'v': 'verbose output'
	}
	epl: str = """examples:
	python ep1_libgen.py -s Lib.sym -o elfloader.lib
	python ep1_libgen.py -sn -s Lib.sym -o elfloader.lib

	python ep1_libgen.py -s Lib.sym -o libstd.a
	python ep1_libgen.py -sn -s Lib.sym -o libstd.a

	python ep1_libgen.py -s elfloader.lib -pf 'E1_R373_G_0E.30.49R' -o Lib.sym
	python ep1_libgen.py -sn -s elfloader.lib -pf 'E1_R373_G_0E.30.49R' -o Lib.sym

	python ep1_libgen.py -s Lib.sym -o Lib.asm
	python ep1_libgen.py -s Lib.sym -o Lib.o
	"""
	parser_args: Args = Args(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-s', '--source', required=True, type=forge.at_file, metavar='INPUT', help=hlp['s'])
	parser_args.add_argument('-o', '--output', required=True, type=forge.at_path, metavar='OUTPUT', help=hlp['o'])
	parser_args.add_argument('-pf', '--phone-fw', required=False, type=forge.at_pfw, metavar='PHONE_FW', help=hlp['pf'])
	parser_args.add_argument('-sa', '--sort-address', required=False, action='store_true', help=hlp['sa'])
	parser_args.add_argument('-st', '--sort-type', required=False, action='store_true', help=hlp['st'])
	parser_args.add_argument('-sn', '--sort-name', required=False, action='store_true', help=hlp['sn'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, sort, args = parse_arguments()

	forge.set_logging_configuration(args.verbose)

	start_ep1_libgen_work(mode, sort, args)
	if mode == Mode.ASSEMBLER_LISTING:
		logging.debug(f'')
		forge.dump_text_file_to_debug_log(args.output, strip_lines=False)


if __name__ == '__main__':
	main()
