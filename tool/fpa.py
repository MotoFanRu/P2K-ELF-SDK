#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Utility for generating Flash&Backup 3 patches in the *.fpa format.

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
	MODE_HEX = 0
	MODE_BIN = 1
	MODE_CONVERT = 2
	MODE_UNITE = 3


# Various generators.

# PortKit working flow.
def start_fpa_work(mode: Mode, args: Namespace) -> bool:
	logging.info(f'Start FPA patcher utility, mode: {mode.name}.')
	return True


# Arguments parsing routines.
class ArgsParser(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')

	@staticmethod
	def check_arguments(args: Namespace, empty_args: list[str], non_empty_args: list[str]) -> bool:
		items = vars(args)
		empty = 0
		for k in empty_args:
			if items[k] is None:
				empty += 1
		non_empty = 0
		for k in non_empty_args:
			if items[k] is not None:
				non_empty += 1
		return (len(empty_args) == empty) and (len(non_empty_args) == non_empty)

	def parse_check_arguments(self) -> tuple[Mode, Namespace]:
		args = self.parse_args()
		check_mode_fpa_hex = self.check_arguments(
			args,
			['bin', 'convert', 'uni'],
			['output', 'firmware', 'author', 'desc', 'start', 'hex', 'verbose']
		)
		check_mode_fpa_bin = self.check_arguments(
			args,
			['hex', 'convert', 'uni'],
			['output', 'firmware', 'author', 'desc', 'start', 'bin', 'verbose']
		)
		check_mode_fpa_convert = self.check_arguments(
			args,
			['firmware', 'author', 'desc', 'start', 'hex', 'bin', 'undo', 'uni'],
			['output', 'convert', 'verbose']
		)
		check_mode_fpa_unite = self.check_arguments(
			args,
			['firmware', 'author', 'desc', 'start', 'hex', 'bin', 'undo', 'convert'],
			['output', 'uni', 'verbose']
		)
		if check_mode_fpa_hex:
			return Mode.MODE_HEX, args
		elif check_mode_fpa_bin:
			return Mode.MODE_BIN, args
		elif check_mode_fpa_convert:
			return Mode.MODE_CONVERT, args
		elif check_mode_fpa_unite:
			if len(args.uni) > 1:
				return Mode.MODE_UNITE, args
			else:
				self.error('needs more than one patch for unite')
		else:
			self.error('all arguments are empty')


def parse_arguments() -> tuple[Mode, Namespace]:
	hlp = {
		'h': 'Utility for generating Flash&Backup 3 patches in the *.fpa format, 15-Dec-2023',
		'o': 'output resulting file',
		'f': 'firmware tuple string, e.g. "R373_G_0E.30.49R"',
		'a': 'author name or nickname, e.g. "EXL"',
		'd': 'simple patch description',
		's': 'start offset address, e.g. "0x10080000"',
		'x': 'hex data string which will be written to offset, e.g. "0123456789ABCDEF"',
		'b': 'binary file which will be written to offset, e.g. "ElfPack.bin"',
		'u': 'generate UNDOs patch information, CG1.smg is needed, e.g. "E1_R373_G_0E.30.49R.smg"',
		'c': 'convert *.fpa patch to binary code',
		'i': 'combine all fpa patches to united one, e.g. "Patch1.fpa", "Patch2.fpa", "Patch3.fpa"',
		'v': 'verbose output'
	}
	epl = """examples:
	python fpa.py -f "R373_G_0E.30.49R" -a "EXL" -d "ElfPack v1.0" -s 0x10080000 -b ElfPack.bin -o Result.fpa
	python fpa.py -f "R373_G_0E.30.49R" -a "EXL" -d "Description" -s 0x10080000 -x "0123456789ABCDEF" -o Result.fpa
	python fpa.py -f "R373_G_0E.30.49R" -a "EXL" -d "ElfPack v1.0" -s 0x10080000 -b ElfPack.bin -u CG1.smg -o Result.fpa
	python fpa.py -f "R373_G_0E.30.49R" -a "EXL" -d "Description" -s 0x10080000 -x "A0B1C3" -u CG1.smg -o Result.fpa
	python fpa.py -c ElfPack.fpa -o Result.bin
	python fpa.py -i ElfPack.fpa Register.fpa -o Result.fpa
	"""
	parser_args = ArgsParser(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-o', '--output', required=False, type=forge.at_fpa, metavar='FILE', help=hlp['o'])
	parser_args.add_argument('-f', '--firmware', required=False, type=str, metavar='FIRMWARE', help=hlp['f'])
	parser_args.add_argument('-a', '--author', required=False, type=str, metavar='AUTHOR', help=hlp['a'])
	parser_args.add_argument('-d', '--desc', required=False, type=str, metavar='DESCRIPTION', help=hlp['d'])
	parser_args.add_argument('-s', '--start', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['s'])
	parser_args.add_argument('-x', '--hex', required=False, type=forge.at_hds, metavar='HEX_DATA_STR', help=hlp['x'])
	parser_args.add_argument('-b', '--bin', required=False, type=forge.at_file, metavar='BINARY_FILE', help=hlp['b'])
	parser_args.add_argument('-u', '--undo', required=False, type=forge.at_file, metavar='FIRMWARE_FILE', help=hlp['u'])
	parser_args.add_argument('-c', '--convert', required=False, type=forge.at_fpac, metavar='FPA', help=hlp['c'])
	parser_args.add_argument('-i', '--uni', required=False, nargs='+', type=forge.at_fpac, metavar='FPA', help=hlp['i'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, args = parse_arguments()

	logging.basicConfig(
		level=logging.DEBUG if args.verbose else logging.INFO,
		format='%(asctime)s %(levelname)s: %(message)s',
		datefmt='%d-%b-%Y %H:%M:%S'
	)

	start_fpa_work(mode, args)


if __name__ == '__main__':
	main()
