#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A patcher utility for Motorola phones on P2K platform.

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
	MODE_WRITE = 2
	MODE_CONVERT = 3
	MODE_UNITE = 4


# Helpers.
def dump_output_patch(fpa: Path) -> None:
	if fpa and fpa.is_file() and fpa.exists():
		with fpa.open(mode='r') as f_i:
			for line in f_i.readlines():
				logging.debug(f'{line.strip()}')


def is_undo_here(undo: Path) -> bool:
	if undo and undo.is_file() and undo.exists():
		logging.info(f'Undo patch source "{undo}" activated.')
		return True
	logging.info(f'Undo patch source deactivated.')
	return False


def log(result: bool) -> bool:
	logging.info(f'Done.' if result else f'Fail.')
	return result


# PortKit working flow.
def start_patcher_work(mode: Mode, args: Namespace) -> bool:
	logging.info(f'Start patcher utility, mode: {mode.name}.')
	if mode == Mode.MODE_BIN:
		is_undo_here(args.undo)
		logging.info(f'Will insert "{args.bin}" to "{forge.int2hex(args.start)}"...')
		logging.info(f'Patch "{args.output}" will be generated.')
		return log(forge.bin2fpa(args.firmware, args.author, args.desc, args.start, args.bin, args.output, args.undo))
	elif mode == Mode.MODE_HEX:
		is_undo_here(args.undo)
		logging.info(f'Will insert "{forge.chop_string_to_16_symbols(args.hex)}" to "{forge.int2hex(args.start)}"...')
		logging.info(f'Patch "{args.output}" will be generated.')
		return log(forge.hex2fpa(args.firmware, args.author, args.desc, args.start, args.hex, args.output, args.undo))
	elif mode == Mode.MODE_WRITE:
		is_undo_here(args.undo)
		logging.info(f'Will write "{args.write}" to "{args.undo}"...')
		return log(forge.apply_fpa_patch(args.undo, args.write, True, True))
	elif mode == Mode.MODE_CONVERT:
		logging.info(f'Will convert "{args.convert}" to "{args.output}"...')
		return log(forge.fpa2bin(args.convert, args.output))
	elif mode == Mode.MODE_UNITE:
		for name in args.uni:
			logging.info(f'Will unite "{name}" to "{args.output}"...')
		return log(forge.unite_fpa_patches(args.firmware, args.author, args.desc, args.uni, args.output))
	else:
		logging.error(f'Unknown mode: {mode.name}')
	return False


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
		check_mode_hex = self.check_arguments(
			args,
			['bin', 'convert', 'uni', 'write'],
			['output', 'firmware', 'author', 'desc', 'start', 'hex', 'verbose']
		)
		check_mode_bin = self.check_arguments(
			args,
			['hex', 'convert', 'uni', 'write'],
			['output', 'firmware', 'author', 'desc', 'start', 'bin', 'verbose']
		)
		check_mode_write = self.check_arguments(
			args,
			['firmware', 'author', 'desc', 'start', 'hex', 'bin', 'output', 'convert', 'uni'],
			['undo', 'write', 'verbose']
		)
		check_mode_convert = self.check_arguments(
			args,
			['firmware', 'author', 'desc', 'start', 'hex', 'bin', 'undo', 'write', 'uni'],
			['output', 'convert', 'verbose']
		)
		check_mode_unite = self.check_arguments(
			args,
			['start', 'hex', 'bin', 'undo', 'write', 'convert'],
			['firmware', 'author', 'desc', 'output', 'uni', 'verbose']
		)
		if check_mode_hex:
			return Mode.MODE_HEX, args
		elif check_mode_bin:
			return Mode.MODE_BIN, args
		elif check_mode_convert:
			return Mode.MODE_CONVERT, args
		elif check_mode_write:
			return Mode.MODE_WRITE, args
		elif check_mode_unite:
			if len(args.uni) > 1:
				return Mode.MODE_UNITE, args
			else:
				self.error('needs more than one patch for unite')
		else:
			self.error('all arguments are empty')


def parse_arguments() -> tuple[Mode, Namespace]:
	hlp = {
		'h': 'A patcher utility for Motorola phones on P2K platform, 15-Dec-2023',
		'o': 'output resulting file',
		'f': 'firmware tuple string, e.g. "R373_G_0E.30.49R"',
		'a': 'author name or nickname, e.g. "EXL"',
		'd': 'simple patch description',
		's': 'start offset address in file, e.g. "0x00080000"',
		'x': 'hex data string which will be written to offset, e.g. "0123456789ABCDEF"',
		'b': 'binary file which will be written to offset, e.g. "ElfPack.bin"',
		'u': 'generate UNDOs patch information, CG1.smg is needed, e.g. "E1_R373_G_0E.30.49R.smg"',
		'w': 'apply and write patch to the firmware file',
		'c': 'convert FPA-patch to binary code',
		'i': 'combine all FPA-patches to united one, e.g. "Patch1.fpa", "Patch2.fpa", "Patch3.fpa"',
		'v': 'verbose output'
	}
	epl = """examples:
	python patch.py -f "R373_G_0E.30.49R" -a "EXL" -d "ElfPack v1.0" -s 0x00080000 -b ElfPack.bin -o Result.fpa
	python patch.py -f "R373_G_0E.30.49R" -a "EXL" -d "Description" -s 0x00080000 -x "0123456789ABCDEF" -o Result.fpa
	python patch.py -f "R373_G_0E.30.49R" -a "EXL" -d "Description" -s 0x00080000 -b File.bin -u CG1.smg -o Result.fpa
	python patch.py -f "R373_G_0E.30.49R" -a "EXL" -d "Description" -s 0x00080000 -x "A0B1C3" -u CG1.smg -o Result.fpa
	python patch.py -w Result.fpa -u CG1.smg
	python patch.py -c ElfPack.fpa -o Result.bin
	python patch.py -f "R373_G_0E.30.49R" -a "EXL" -d "United Patches" -i ElfPack.fpa Register.fpa -o Result.fpa
	"""
	parser_args = ArgsParser(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-o', '--output', required=False, type=forge.at_path, metavar='OUTPUT', help=hlp['o'])
	parser_args.add_argument('-f', '--firmware', required=False, type=str, metavar='FIRMWARE', help=hlp['f'])
	parser_args.add_argument('-a', '--author', required=False, type=str, metavar='AUTHOR', help=hlp['a'])
	parser_args.add_argument('-d', '--desc', required=False, type=str, metavar='DESCRIPTION', help=hlp['d'])
	parser_args.add_argument('-s', '--start', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['s'])
	parser_args.add_argument('-x', '--hex', required=False, type=forge.at_hds, metavar='HEX_DATA_STR', help=hlp['x'])
	parser_args.add_argument('-b', '--bin', required=False, type=forge.at_file, metavar='BINARY_FILE', help=hlp['b'])
	parser_args.add_argument('-u', '--undo', required=False, type=forge.at_file, metavar='FIRMWARE_FILE', help=hlp['u'])
	parser_args.add_argument('-w', '--write', required=False, type=forge.at_fpa, metavar='FPA', help=hlp['w'])
	parser_args.add_argument('-c', '--convert', required=False, type=forge.at_fpa, metavar='FPA', help=hlp['c'])
	parser_args.add_argument('-i', '--uni', required=False, nargs='+', type=forge.at_fpa, metavar='FPA', help=hlp['i'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, args = parse_arguments()

	logging.basicConfig(
		level=logging.DEBUG if args.verbose else logging.INFO,
		format='%(asctime)s %(levelname)s: %(message)s',
		datefmt='%d-%b-%Y %H:%M:%S'
	)

	start_patcher_work(mode, args)
	if (mode == Mode.MODE_BIN) or (mode == Mode.MODE_HEX) or (mode == Mode.MODE_UNITE):
		logging.debug(f'')
		dump_output_patch(args.output)


if __name__ == '__main__':
	main()
