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


# Helpers.


# Forge working flow.
def start_forge_work(mode: Mode, args: Namespace) -> bool:
	logging.info(f'Start Forge auxiliary utility, mode: "{mode.name}".')
	if mode == Mode.SYM_TO_PAT:
		return forge.log_result(
			forge.sym2pat(args.source, args.output, args.firmware, args.offset, args.size, args.irom)
		)
	return False


# Arguments parsing routines.
class Args(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')

	def parse_check_arguments(self) -> tuple[Mode, Namespace]:
		args: Namespace = self.parse_args()

		s: Path = args.source
		o: Path = args.output
		f: Path = args.firmware
		g: int = args.offset
		z: int = args.size

		s_sym = forge.check_files_extensions([s], ['sym'], False)
		o_pat = forge.check_files_extensions([o], ['pat'], False)

		if s_sym and o_pat and (f is not None) and (g is not None) and (z is not None):
			return Mode.SYM_TO_PAT, args

		self.error('all arguments are empty')


def parse_arguments() -> tuple[Mode, Namespace]:
	hlp: dict[str, str] = {
		'h': 'A Forge auxiliary utility for various ElfPacks and Motorola phones on P2K platform, 15-Dec-2023',
		's': 'source file',
		'f': 'path to CG0+CG1 firmware file',
		'g': 'offset (in HEX)',
		'z': 'size (integer)',
		'o': 'output file',
		'i': 'irom',
		'v': 'verbose output'
	}
	epl: str = """examples:
	# Generate a draft patterns file from symbols file (+irom).
	python forge.py -s library.sym -f cg/E1_R373_G_0E.30.49R.smg -g 0x10080000 -z 16 -o patterns.pat
	python forge.py -i -s library.sym -f irom/E1_R373_G_0E.30.49R.smg -g 0x10080000 -z 16 -o patterns.pat
	"""
	parser_args: Args = Args(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-s', '--source', required=True, type=forge.at_file, metavar='INPUT', help=hlp['s'])
	parser_args.add_argument('-o', '--output', required=True, type=forge.at_path, metavar='OUTPUT', help=hlp['o'])
	parser_args.add_argument('-f', '--firmware', required=False, type=forge.at_file, metavar='FILE.smg', help=hlp['f'])
	parser_args.add_argument('-g', '--offset', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['g'])
	parser_args.add_argument('-z', '--size', required=False, type=forge.at_int, metavar='SIZE', help=hlp['z'])
	parser_args.add_argument('-i', '--irom', required=False, action='store_true', help=hlp['i'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, args = parse_arguments()

	forge.set_logging_configuration(args.verbose)

	start_forge_work(mode, args)


if __name__ == '__main__':
	main()
