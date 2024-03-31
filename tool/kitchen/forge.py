#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A Forge auxiliary utility for various ElfPacks and Motorola phones on P2K platform.

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
	SYM_TO_PAT: int = 0
	SYM_TO_SYM: int = 1


# Helpers.


# Forge working flow.
def start_forge_work(mode: Mode,  sort: forge.LibrarySort, args: Namespace) -> bool:
	logging.info(f'Start Forge auxiliary utility, mode: "{mode.name}".')
	if mode == Mode.SYM_TO_PAT:
		return forge.log_result(
			forge.sym2pat(args.source, args.output, args.firmware, args.offset, args.size, args.irom)
		)
	elif mode == Mode.SYM_TO_SYM:
		names: list[str] | None = forge.libgen_names_sym(args.defines, args.elfpack, not args.const)
		if names:
			return forge.log_result(
				forge.libgen_chunk_sym(args.source, args.output, sort, names, args.phone_fw, args.elfpack)
			)
		else:
			logging.error(f'Names list of {args.defines} file are empty!')
	return False


# Arguments parsing routines.
class Args(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')

	def parse_check_arguments(self) -> tuple[Mode, forge.LibrarySort, Namespace]:
		args: Namespace = self.parse_args()
		sort: forge.LibrarySort = forge.determine_sort_mode(args)

		s: Path = args.source
		o: Path = args.output
		f: Path = args.firmware
		g: int = args.offset
		z: int = args.size
		d: Path = args.defines
		e: forge.ElfPack = args.elfpack
		pf: tuple[str, str] = args.phone_fw

		s_sym: bool = forge.check_files_extensions([s], ['sym'], False)
		o_pat: bool = forge.check_files_extensions([o], ['pat'], False)
		o_sym: bool = forge.check_files_extensions([o], ['sym'], False)
		d_sym: bool = forge.check_files_extensions([d], ['sym', 'def'], False)

		if s_sym and o_pat and (f is not None) and (g is not None) and (z is not None):
			return Mode.SYM_TO_PAT, sort, args
		elif s_sym and d_sym and o_sym and pf and (e is not None):
			return Mode.SYM_TO_SYM, sort, args

		self.error('all arguments are empty')


def parse_arguments() -> tuple[Mode, forge.LibrarySort, Namespace]:
	hlp: dict[str, str] = {
		'h': 'A Forge auxiliary utility for various ElfPacks and Motorola phones on P2K platform, 15-Dec-2023',
		's': 'source file',
		'f': 'path to CG0+CG1 firmware file',
		'g': 'offset (in HEX)',
		'z': 'size (integer)',
		'o': 'output file',
		'i': 'irom',
		'pf': 'phone and firmware, e.g. "E1_R373_G_0E.30.49R"',
		'd': 'source defines or symbols file',
		'e': 'ElfPack version',
		'sa': 'sort by addresses',
		'st': 'sort by types',
		'sn': 'sort by names',
		'c': 'activate constants',
		'v': 'verbose output'
	}
	epl: str = """examples:
	# Generate a draft patterns file from symbols file (+irom).
	python forge.py -s library.sym -f ../cg/E1_R373_G_0E.30.49R.smg -g 0x10080000 -z 32 -o patterns.pat
	python forge.py -i -s library.sym -f ../irom/0300-irom-LTE2.bin -g 0x00000000 -z 32 -o patterns.pat

	# Rechunk symbols file from another one.
	python forge.py -sn -s gsm_flash_dev.sym -d elfloader.sym -e EP1 -pf 'E1_R373_G_0E.30.49R' -o library.sym
	python forge.py -sn -c -s gsm_flash_dev.sym -d elfloader.sym -e EP1 -pf 'E1_R373_G_0E.30.49R' -o library.sym
	"""
	parser_args: Args = Args(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-s', '--source', required=True, type=forge.at_file, metavar='INPUT', help=hlp['s'])
	parser_args.add_argument('-o', '--output', required=True, type=forge.at_path, metavar='OUTPUT', help=hlp['o'])
	parser_args.add_argument('-f', '--firmware', required=False, type=forge.at_file, metavar='FILE.smg', help=hlp['f'])
	parser_args.add_argument('-g', '--offset', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['g'])
	parser_args.add_argument('-z', '--size', required=False, type=forge.at_int, metavar='SIZE', help=hlp['z'])
	parser_args.add_argument('-i', '--irom', required=False, action='store_true', help=hlp['i'])
	parser_args.add_argument('-pf', '--phone-fw', required=False, type=forge.at_pfw, metavar='PHONE_FW', help=hlp['pf'])
	parser_args.add_argument('-d', '--defines', required=False, type=forge.at_file, metavar='INPUT', help=hlp['d'])
	parser_args.add_argument('-e', '--elfpack', required=False, type=forge.at_ep, metavar='ELFPACK', help=hlp['e'])
	parser_args.add_argument('-sa', '--sort-address', required=False, action='store_true', help=hlp['sa'])
	parser_args.add_argument('-st', '--sort-type', required=False, action='store_true', help=hlp['st'])
	parser_args.add_argument('-sn', '--sort-name', required=False, action='store_true', help=hlp['sn'])
	parser_args.add_argument('-c', '--const', required=False, action='store_true', help=hlp['c'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, sort, args = parse_arguments()

	forge.set_logging_configuration(args.verbose)

	start_forge_work(mode, sort, args)


if __name__ == '__main__':
	main()
