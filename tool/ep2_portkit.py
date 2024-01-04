#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A PortKit Utility for building ElfPack v2.0 for Motorola phones on P2K platform.

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

from pathlib import Path
from datetime import datetime
from argparse import Namespace


# Constants.
EP2_PFW_VARIANTS: dict[str, dict[str, any]] = {
	'R373_G_0E.30.49R': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_E398', '-DFTR_PHONE_NAME="E398"'],
		'opts_firmware':  ['-DR373_G_0E_30_49R', '-DFTR_PHONE_PLATFORM="LTE"'],
		'opts_keyboard':  ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10080000,  # Firmware start address.
		'addr_ep2_body':  0x00C73000,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0025A434,  # Register patch address.
		'addr_upd_disp':  0x00028DF0,  # Update Display patch address.
		# DataLogger block 0x12200000-0x122008CC (Andy51).
		'addr_ram_block': 0x124FD320   # At least ~0x400 free bytes block in RAM, alternate: 0x122008E0.
	},
	'R373_G_0E.30.70R': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_E398', '-DFTR_PHONE_NAME="E398"'],
		'opts_firmware':  ['-DR373_G_0E_30_70R', '-DFTR_PHONE_PLATFORM="LTE"'],
		'opts_keyboard':  ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10080000,  # Firmware start address.
		'addr_ep2_body':  0x00C73000,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0025D0B0,  # Register patch address.
		'addr_upd_disp':  0x00029504,  # Update Display patch address.
		'addr_ram_block': 0x125015D8   # At least ~0x400 free bytes block in RAM.
	},
	'R373_G_0E.30.79R': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_E398', '-DFTR_PHONE_NAME="E398"'],
		'opts_firmware':  ['-DR373_G_0E_30_79R', '-DFTR_PHONE_PLATFORM="LTE"'],
		'opts_keyboard':  ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10080000,  # Firmware start address.
		'addr_ep2_body':  0x00C73000,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0025CC50,  # Register patch address.
		'addr_upd_disp':  0x00029914,  # Update Display patch address.
		'addr_ram_block': 0x12502538   # At least ~0x400 free bytes block in RAM.
	},
	'R452B_G_08.02.0DR': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_L7E', '-DFTR_PHONE_NAME="Z3"'],
		'opts_firmware':  ['-DR452B_G_08_02_0DR', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  [],          # ['-DFTR_KEYPAD_TYPE=KP_TWO_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x01456000,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0118AD54,  # Register patch address.
		'addr_upd_disp':  0x002E6328,  # Update Display patch address.
		# Ram_l7e D [7FFF0000011E00000122+0x0A], probably wrong alternative 0x145D5200 value (Andy51).
		# Ram_l7e D [2014900020009001900290039004480E+0x2A]+6C, 0x14073328 or 0x1407332C but probably first (Andy51).
		'addr_ram_block': 0x14073328   # At least ~0x400 free bytes block in RAM.
	},
	'R452D_G_08.01.0AR': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_L7E', '-DFTR_PHONE_NAME="L7e"'],
		'opts_firmware':  ['-DR452D_G_08_01_0AR', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  [],  # ['-DFTR_KEYPAD_TYPE=KP_TWO_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x01430000,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x01253FEC,  # Register patch address.
		'addr_upd_disp':  0x0028E148,  # Update Display patch address.
		# Ram_l7e D [7FFF0000011E00000122+0x0A], probably wrong alternative 0x145C96C8 value (Andy51).
		# Ram_l7e D [2014900020009001900290039004480E+0x2A]+6C.
		'addr_ram_block': 0x140737B0  # At least ~0x400 free bytes block in RAM.
	},
	'R452F1_G_08.04.09R': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_L7E', '-DFTR_PHONE_NAME="Z3"'],
		'opts_firmware':  ['-DR452F1_G_08_04_09R', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  [],          # ['-DFTR_KEYPAD_TYPE=KP_TWO_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x0153C000,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0118B0C8,  # Register patch address.
		'addr_upd_disp':  0x002CD2B0,  # Update Display patch address.
		# Ram_l7e D [7FFF0000011E00000122+0x0A], probably wrong alternative 0x1453EC28 value (Andy51).
		# Ram_l7e D [14??????00003E580000FFFF]+0x4.
		'addr_ram_block': 0x14076374   # At least ~0x400 free bytes block in RAM.
	},
	'R452F_G_08.03.08R': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_L7E', '-DFTR_PHONE_NAME="K1"'],
		'opts_firmware':  ['-DR452F_G_08_03_08R', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  [],          # ['-DFTR_KEYPAD_TYPE=KP_TWO_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x01530000,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x011EB174,  # Register patch address.
		'addr_upd_disp':  0x00244310,  # Update Display patch address.
		# Ram_l7e D [7FFF0000011E00000122+0x0A], probably wrong alternative 0x1402B2A4 value (Andy51).
		# Ram_l7e D [201490002000900190029003+0x01E8]+28
		'addr_ram_block': 0x14501210   # At least ~0x400 free bytes block in RAM.
	},
	'R452H6_G_08.00.05R': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_L7E', '-DFTR_PHONE_NAME="Z3"'],
		'opts_firmware':  ['-DR452H6_G_08_00_05R', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  [],          # ['-DFTR_KEYPAD_TYPE=KP_TWO_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x0145B000,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0118B0FC,  # Register patch address.
		'addr_upd_disp':  0x002E6228,  # Update Display patch address.
		# Ram_l7e D [7FFF0000011E00000122+0x0A], probably wrong alternative 0x145D5200 value (Andy51).
		# Ram_l7e D [2014900020009001900290039004480E+0x2A]+6C, 0x14073328 or 0x1407332C but probably first (Andy51).
		'addr_ram_block': 0x14073328   # At least ~0x400 free bytes block in RAM.
	},
	'R452J_G_08.22.05R': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_L9', '-DFTR_PHONE_NAME="L9"'],
		'opts_firmware':  ['-DR452J_G_08_22_05R', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  [],          # ['-DFTR_KEYPAD_TYPE=KP_TWO_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x0153119C,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0121E0CC,  # Register patch address.
		'addr_upd_disp':  0x00236C6C,  # Update Display patch address.
		# 0x1451C1C8  0x3e9  uis_data_logger_buffer
		# Ram_l7e D [7FFF0000011E00000122+0x0A]
		'addr_ram_block': 0x1451C1C8   # At least ~0x400 free bytes block in RAM.
	},
	'R4441D_G_08.01.03R': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_V3i', '-DFTR_PHONE_NAME="V3i"'],
		'opts_firmware':  ['-DR4441D_G_08_01_03R', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x00CDAE20,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0025C460,  # Register patch address.
		'addr_upd_disp':  0x00014864,  # Update Display patch address.
		'addr_ram_block': 0x1446428C   # At least ~0x400 free bytes block in RAM.
	},
	'R4441D_G_08.03.05R': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_V3i', '-DFTR_PHONE_NAME="V3i"'],
		'opts_firmware':  ['-DR4441D_G_08_03_05R', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x00D00000,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x00266BDC,  # Register patch address.
		'addr_upd_disp':  0x00014BA4,  # Update Display patch address.
		'addr_ram_block': 0x1446DDF4   # At least ~0x400 free bytes block in RAM.
	},
	'R4513_G_08.B7.ACR': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_V360', '-DFTR_PHONE_NAME="V360"'],
		'opts_firmware':  ['-DR4513_G_08_B7_ACR', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x00C8D000,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0027DD30,  # Register patch address.
		'addr_upd_disp':  0x00014568,  # Update Display patch address.
		'addr_ram_block': 0x124EB7B0   # At least ~0x400 free bytes block in RAM.
	},
	'R4513_G_08.B7.ACR_RB': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_L7', '-DFTR_PHONE_NAME="L7"'],
		'opts_firmware':  ['-DR4513_G_08_B7_ACR_RB', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x00C8D630,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0027DD30,  # Register patch address.
		'addr_upd_disp':  0x00014568,  # Update Display patch address.
		'addr_ram_block': 0x124EBBA0  # At least ~0x400 free bytes block in RAM.
	},
	'R4513_G_08.B7.E0R_RB': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_L7', '-DFTR_PHONE_NAME="L7"'],
		'opts_firmware':  ['-DR4513_G_08_B7_E0R_RB', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x00C8D630,  # ElfPack v2.0 offset address. TODO: ???
		'addr_ep2_reg':   0x0027DD30,  # Register patch address. TODO: ???
		'addr_upd_disp':  0x00014568,  # Update Display patch address. TODO: ???
		'addr_ram_block': 0x124EBBA0  # At least ~0x400 free bytes block in RAM. TODO: ???
	},
	'R4515_G_08.BD.D3R': {
		'opts_main':      ['-DPATCH', '-DDEBUG'],
		'opts_phone':     ['-DFTR_V3r', '-DFTR_PHONE_NAME="V3r"'],
		'opts_firmware':  ['-DR4515_G_08_BD_D3R', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'opts_keyboard':  ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0'],
		'opts_keyfast':   ['-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'opts_debug':     ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'addr_fw_start':  0x10092000,  # Firmware start address.
		'addr_ep2_body':  0x00C60EF0,  # ElfPack v2.0 offset address.
		'addr_ep2_reg':   0x0025E6D8,  # Register patch address.
		'addr_upd_disp':  0x0001CF2C,  # Update Display patch address.
		'addr_ram_block': 0x144C7C68   # At least ~0x400 free bytes block in RAM.
	}
}

# Various generators.


# PortKit ARM v2.0 working flow.
def start_ep2_portkit_work(opts: dict[str, any]) -> bool:
	logging.info('Start building ElfPack v2.0 for Motorola P2K.')
	logging.info('')
	logging.info('Parameters:')
	forge.args_dump(opts)
	logging.info('')
	logging.info('Prepare PortKit environment.')
	forge.prepare_clean_output_directory(opts["output"], opts['clean'])
	logging.info('')

	logging.info('Compiling C-source files using ADS compiler.')
	c_sources: list[str] = [
		'API.c',
		'autorun.c',
		'config.c',
		'console.c',
		'dd.c',
		'dispCallBacks.c',
		'elfloaderApp.c',
		'font.c',
		'loadCommon.c',
		'loadDefLib.c',
		'loadElf.c',
		'loadLibrary.c',
		'logo.c',
		'lolvl.c',
		'palette.c',
		'parser.c',
		'utils.c'
	]
	for c_source in c_sources:
		p_c: Path = forge.P2K_DIR_EP2_SRC / c_source
		p_o: Path = opts['output'] / (c_source + '.o')
		if not forge.ep1_ads_tcc(p_c, p_o, True, opts['flags']):
			return False
	logging.info('')

	return True


# Arguments parsing routines.
class Args(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')

	def parse_check_arguments(self) -> dict[str, any]:
		opts: dict[str, any] = {}
		args: Namespace = self.parse_args()

		phone, firmware = args.phone_fw
		variants: dict[str, any] = EP2_PFW_VARIANTS.get(firmware, None)
		if not variants:
			self.error(f'unknown {phone} phone and {firmware} firmware')
		sym_source_file: Path = forge.P2K_DIR_LIB / '_'.join(args.phone_fw) / 'library.sym'
		if not forge.check_files_if_exists([sym_source_file]):
			self.error(f'cannot find {sym_source_file} file with entity addresses')

		opts['verbose'] = args.verbose
		opts['clean'] = args.clean
		opts['output'] = args.output
		opts['debug'] = args.debug
		opts['directory'] = args.directory

		opts['start'] = args.start if args.start else variants['addr_fw_start']
		opts['offset'] = args.offset if args.offset else variants['addr_ep2_body']
		opts['register'] = args.register if args.register else variants['addr_ep2_reg']
		opts['display'] = args.display if args.display else variants['addr_upd_disp']
		opts['block'] = args.block if args.block else variants['addr_ram_block']
		
		flags: list[str] = (
			variants['opts_main'] +
			variants['opts_phone'] +
			variants['opts_firmware'] +
			variants['opts_keyboard'] +
			variants['opts_keyfast']
		)
		if args.debug:
			flags += variants['opts_debug']
		opts['flags'] = flags

		opts['soc'] = forge.determine_soc(opts['start'])
		opts['phone'] = phone
		opts['fw_name'] = firmware
		opts['sym'] = sym_source_file

		opts['addr_main'] = opts['start'] + opts['offset']
		opts['addr_disp'] = opts['start'] + opts['display']

		return opts


def parse_arguments() -> dict[str, any]:
	hlp: dict[str, str] = {
		'd': 'A PortKit Utility for building ElfPack v2.0 for Motorola phones on P2K platform, 15-Dec-2023',
		'c': 'clean output directory before processing',
		'pf': 'phone and firmware, e.g. "E1_R373_G_0E.30.49R"',
		's': 'override start address of CG0+CG1 firmware (in HEX)',
		'g': 'override result patch offset in CG0+CG1 file (in HEX)',
		'r': 'override register address (in HEX)',
		'j': 'override display inject address (in HEX)',
		'b': 'override block RAM address (in HEX)',
		'o': 'output artifacts directory',
		'db': 'debug build of ElfPack v2.0',
		't': 'generate patch with replacing "ringtone" to "Elf" directory',
		'v': 'verbose output'
	}
	epl: str = """examples:
	# Build ElfPack v2.0 to the phone/firmware using source code (+'Elf' directory patch).
	python ep2_portkit.py -c -t -pf E1_R373_G_0E.30.49R -o build
	# TODO: Fill it with phones and models.
	"""
	parser_args: Args = Args(description=hlp['d'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-c', '--clean', required=False, action='store_true', help=hlp['c'])
	parser_args.add_argument('-pf', '--phone-fw', required=True, type=forge.at_pfw, metavar='PHONE_FW', help=hlp['pf'])
	parser_args.add_argument('-s', '--start', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['s'])
	parser_args.add_argument('-g', '--offset', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['g'])
	parser_args.add_argument('-r', '--register', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['r'])
	parser_args.add_argument('-j', '--display', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['j'])
	parser_args.add_argument('-b', '--block', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['b'])
	parser_args.add_argument('-o', '--output', required=True, type=forge.at_path, metavar='DIRECTORY', help=hlp['o'])
	parser_args.add_argument('-d', '--debug', required=False, action='store_true', help=hlp['db'])
	parser_args.add_argument('-t', '--directory', required=False, action='store_true', help=hlp['t'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	start_time: datetime = datetime.now()
	args: dict[str, any] = parse_arguments()

	forge.set_logging_configuration(args['verbose'])

	start_ep2_portkit_work(args)

	time_elapsed: str = forge.format_timedelta(datetime.now() - start_time)
	logging.info(f'Time elapsed: "{time_elapsed}".')


if __name__ == '__main__':
	main()
