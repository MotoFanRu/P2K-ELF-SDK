#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A Patcher Utility for Motorola phones on P2K platform.

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
	HEX: int = 0
	BIN: int = 1
	WRITE: int = 2
	CONVERT: int = 3
	UNITE: int = 4


# Helpers.
def is_undo_here(undo: Path) -> None:
	if forge.check_files_if_exists([undo]):
		logging.info(f'Undo patch source "{undo}" activated.')
	logging.info(f'Undo patch source deactivated.')


# Patcher working flow.
def start_patcher_work(mode: Mode, args: Namespace) -> bool:
	logging.info(f'Start patcher utility, mode: {mode.name}.')
	if mode == Mode.BIN:
		is_undo_here(args.undo)
		logging.info(f'Will insert "{args.bin}" to "{forge.int2hex(args.start)}"...')
		logging.info(f'Patch "{args.output}" will be generated.')
		return forge.log_result(
			forge.bin2fpa(args.firmware, args.author, args.desc, args.start, args.bin, args.output, args.undo)
		)
	elif mode == Mode.HEX:
		is_undo_here(args.undo)
		logging.info(f'Will insert "{forge.chop_str(args.hex)}" to "{forge.int2hex(args.start)}"...')
		logging.info(f'Patch "{args.output}" will be generated.')
		return forge.log_result(
			forge.hex2fpa(args.firmware, args.author, args.desc, args.start, args.hex, args.output, args.undo)
		)
	elif mode == Mode.WRITE:
		is_undo_here(args.undo)
		logging.info(f'Will write "{args.write}" to "{args.undo}"...')
		return forge.log_result(forge.apply_fpa_patch(args.undo, args.write, not args.no_backup, args.validate))
	elif mode == Mode.CONVERT:
		logging.info(f'Will convert "{args.convert}" to "{args.output}"...')
		return forge.log_result(forge.fpa2bin(args.convert, args.output))
	elif mode == Mode.UNITE:
		for name in args.uni:
			logging.info(f'Will unite "{name}" to "{args.output}"...')
		return forge.log_result(forge.unite_fpa_patches(args.firmware, args.author, args.desc, args.uni, args.output))
	else:
		logging.error(f'Unknown mode: {mode.name}')
	return False


# Arguments parsing routines.
class Args(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')

	@staticmethod
	def check_arguments(args: Namespace, empty_args: list[str], non_empty_args: list[str]) -> bool:
		items: dict[str, str] = vars(args)
		empty: int = 0
		for k in empty_args:
			if items[k] is None:
				empty += 1
		non_empty: int = 0
		for k in non_empty_args:
			if items[k] is not None:
				non_empty += 1
		return (len(empty_args) == empty) and (len(non_empty_args) == non_empty)

	def parse_check_arguments(self) -> tuple[Mode, Namespace]:
		args: Namespace = self.parse_args()
		check_mode_hex: bool = self.check_arguments(
			args,
			['bin', 'convert', 'uni', 'write'],
			['output', 'firmware', 'author', 'desc', 'start', 'hex', 'verbose', 'no_backup', 'validate']
		)
		check_mode_bin: bool = self.check_arguments(
			args,
			['hex', 'convert', 'uni', 'write'],
			['output', 'firmware', 'author', 'desc', 'start', 'bin', 'verbose', 'no_backup', 'validate']
		)
		check_mode_write: bool = self.check_arguments(
			args,
			['firmware', 'author', 'desc', 'start', 'hex', 'bin', 'output', 'convert', 'uni'],
			['undo', 'write', 'verbose', 'no_backup', 'validate']
		)
		check_mode_convert: bool = self.check_arguments(
			args,
			['firmware', 'author', 'desc', 'start', 'hex', 'bin', 'undo', 'write', 'uni'],
			['output', 'convert', 'verbose', 'no_backup', 'validate']
		)
		check_mode_unite: bool = self.check_arguments(
			args,
			['start', 'hex', 'bin', 'undo', 'write', 'convert'],
			['firmware', 'author', 'desc', 'output', 'uni', 'verbose', 'no_backup', 'validate']
		)
		if check_mode_hex:
			return Mode.HEX, args
		elif check_mode_bin:
			return Mode.BIN, args
		elif check_mode_convert:
			return Mode.CONVERT, args
		elif check_mode_write:
			return Mode.WRITE, args
		elif check_mode_unite:
			if len(args.uni) > 1:
				return Mode.UNITE, args
			else:
				self.error('needs more than one patch for unite')
		else:
			self.error('all arguments are empty')


def parse_arguments() -> tuple[Mode, Namespace]:
	hlp: dict[str, str] = {
		'h': 'A Patcher Utility for Motorola phones on P2K platform, 15-Dec-2023',
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
		'l': 'validate patches if undo data and source is present',
		'n': 'do not backup firmware file before patching',
		'v': 'verbose output'
	}
	epl: str = """examples:
	python patch.py -f "R373_G_0E.30.49R" -a "EXL" -d "ElfPack v1.0" -s 0x00080000 -b ElfPack.bin -o Result.fpa
	python patch.py -f "R373_G_0E.30.49R" -a "EXL" -d "Description" -s 0x00080000 -b File.bin -u CG1.smg -o Result.fpa

	python patch.py -f "R373_G_0E.30.49R" -a "EXL" -d "Description" -s 0x00080000 -x "0123456789ABCDEF" -o Result.fpa
	python patch.py -f "R373_G_0E.30.49R" -a "EXL" -d "Description" -s 0x00080000 -x "A0B1C3" -u CG1.smg -o Result.fpa

	python patch.py -w Result.fpa -u CG1.smg -l
	python patch.py -w Result.fpa -u CG1.smg -l -n

	python patch.py -c ElfPack.fpa -o Result.bin

	python patch.py -f "R373_G_0E.30.49R" -a "EXL" -d "United Patches" -i ElfPack.fpa Register.fpa -o Result.fpa
	"""
	parser_args: Args = Args(description=hlp['h'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
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
	parser_args.add_argument('-l', '--validate', required=False, action='store_true', help=hlp['l'])
	parser_args.add_argument('-n', '--no-backup', required=False, action='store_true', help=hlp['n'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	mode, args = parse_arguments()

	forge.set_logging_configuration(args.verbose)

	start_patcher_work(mode, args)
	if (mode == Mode.BIN) or (mode == Mode.HEX) or (mode == Mode.UNITE):
		logging.debug(f'')
		forge.dump_text_file_to_debug_log(args.output)


if __name__ == '__main__':
	main()
