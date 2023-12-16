#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
PortKit Utility for building ElfPack v1.0 for Motorola phones on P2K platform.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import sys
import logging
import argparse

import forge

from pathlib import Path
from datetime import datetime
from argparse import Namespace


# Constants.
FUNC_INJECTION = 'APP_SyncML_MainRegister'
FUNC_REGISTER = 'Register'
FUNC_AUTORUN = 'AutorunMain'


# Various generators.
def generate_lib_sym(p_i_f: Path, p_i_e: Path, p_o_l: Path, names_skip: list[str], patterns_add: list[str]) -> bool:
	if forge.check_files_if_exists([p_i_f, p_i_e]):
		with (p_i_f.open(mode='r') as i_f, p_i_e.open(mode='r') as i_s, p_o_l.open(mode='w', newline='\r\n') as o_l):
			o_l.write(f'{forge.ADS_SYM_FILE_HEADER}\n')
			o_l.write(f'# SYMDEFS ADS HEADER\n\n')
			for line in i_f.read().splitlines():
				address, mode, name = forge.split_and_validate_line(line)
				if (name is not None) and (name not in names_skip):
					o_l.write(f'{line}\n')
			o_l.write(f'\n\n')
			for line in i_s.read().splitlines():
				address, mode, name = forge.split_and_validate_line(line)
				if name is not None:
					for add in patterns_add:
						if name.find(add) != -1:
							o_l.write(f'{line}\n')
			return True
	return False


def generate_register_patch(fw: str, author: str, desc: str, p_e: Path, p_r: Path, p_p: Path, cg: Path) -> bool:
	if forge.check_files_if_exists([p_e, p_r]):
		hex_data: str = forge.int2hex_r(forge.get_function_address_from_sym_file(p_e, FUNC_AUTORUN) + 1)  # Thumb
		reg_address: int = forge.get_function_address_from_sym_file(p_r, FUNC_REGISTER)

		forge.hex2fpa(fw, author, desc, reg_address, hex_data, p_p, cg)
		return True
	return False


def generate_system_information_source(phone: str, firmware: str, soc: str, source_file: Path) -> bool:
	system_info = {}
	major, minor = forge.parse_minor_major_firmware(firmware)
	system_info['n_phone'] = phone
	system_info['n_platform'] = soc
	system_info['n_majorfw'] = major
	system_info['n_minorfw'] = minor
	return forge.gen_src_const_chars(source_file, system_info)


def generate_register_symbol_file(combined_sym: Path, cgs_path: Path, register_func: str, pat: Path, sym: Path) -> bool:
	address = forge.get_function_address_from_sym_file(combined_sym, register_func)
	if address != 0x00000000:
		forge.pat_append(pat, 'Register', 'D', forge.int2hex_r(address))
		forge.pat_find(pat, cgs_path, 0x00000000, False, sym)
	return False


# PortKit working flow.
def start_port_kit_work(args: Namespace) -> bool:
	logging.info(f'Start building ElfPack v1.0 for Motorola P2K.')
	logging.info(f'')

	arg_verbose = args.verbose
	arg_clean = args.clean
	arg_output = args.output
	arg_patterns = args.patterns
	arg_firmware = args.firmware
	arg_start = args.start
	arg_offset = forge.arrange16(forge.get_file_size(arg_firmware))
	arg_address = arg_start + arg_offset  # Start + Offset.
	arg_ram_trans = args.ram_trans
	arg_fw_name = arg_firmware.name
	arg_soc = forge.determine_soc(arg_start)
	arg_phone, arg_fw = forge.parse_phone_firmware(arg_fw_name)

	logging.info(f'Values:')
	logging.info(f'\targ_verbose={arg_verbose}')
	logging.info(f'\targ_clean={arg_clean}')
	logging.info(f'\targ_output={arg_output}')
	logging.info(f'\targ_patterns={arg_patterns}')
	logging.info(f'\targ_firmware={arg_firmware}')
	logging.info(f'\targ_start=0x{arg_start:08X} {arg_start}')
	logging.info(f'\targ_address=0x{arg_address:08X} {arg_address}')
	logging.info(f'\targ_offset=0x{arg_offset:08X} {arg_offset}')
	logging.info(f'\targ_ram_trans={arg_ram_trans}')
	logging.info(f'\targ_fw_name={arg_fw_name}')
	logging.info(f'\targ_soc={arg_soc}')
	logging.info(f'\targ_phone={arg_phone}')
	logging.info(f'\targ_fw={arg_fw}')
	logging.info(f'')

	logging.info(f'Finding SoC related functions from patterns.')
	val_lte1_pat = forge.P2K_DIR_EP1_FUNC / 'LTE.pat'
	val_lte2_pat = forge.P2K_DIR_EP1_FUNC / 'LTE2.pat'
	val_lte2_irom_sym = forge.P2K_DIR_EP1_FUNC / 'LTE2_IROM.sym'
	val_platform_sym = arg_output / 'Platform.sym'
	val_functions_sym = arg_output / 'Functions.sym'
	val_combined_sym = arg_output / 'Combined.sym'
	if arg_soc == 'LTE':
		forge.pat_find(val_lte1_pat, arg_firmware, arg_start, False, val_platform_sym)
	elif arg_soc == 'LTE2':
		forge.pat_find(val_lte2_pat, arg_firmware, arg_start, False, val_platform_sym)
	else:
		val_functions_sym = val_combined_sym
		logging.warning(f'Unknown SoC platform, will skip generating platform symbols file.')
	logging.info(f'')

	logging.info(f'Finding general functions from patterns.')
	forge.pat_find(arg_patterns, arg_firmware, arg_start, arg_ram_trans, val_functions_sym)
	logging.info(f'')

	logging.info(f'Combining all functions into one symbols file.')
	if arg_soc == 'LTE':
		forge.create_combined_sym_file([val_functions_sym, val_platform_sym], val_combined_sym)
	elif arg_soc == 'LTE2':
		forge.create_combined_sym_file([val_functions_sym, val_combined_sym, val_lte2_irom_sym], val_combined_sym)
	logging.info(f'')

	logging.info(f'Validating combined symbols file.')
	if not forge.validate_sym_file(val_combined_sym):
		return False
	else:
		logging.info(f'The "{val_combined_sym}" sym file is validated.')
	logging.info(f'')

	logging.info(f'Generating register symbols file.')
	val_register_pat = arg_output / 'Register.pat'
	val_register_sym = arg_output / 'Register.sym'
	generate_register_symbol_file(val_combined_sym, arg_firmware, FUNC_INJECTION, val_register_pat, val_register_sym)
	logging.info(f'')

	logging.info(f'Generating system information C-source file.')
	val_system_info_c = arg_output / 'SysInfo.c'
	val_system_info_o = arg_output / 'SysInfo.o'
	generate_system_information_source(arg_phone, arg_fw, arg_soc, val_system_info_c)
	logging.info(f'')

	logging.info(f'Compiling system C-source files.')
	forge.ep1_ads_tcc(val_system_info_c, val_system_info_o)
	logging.info(f'')

	logging.info(f'Linking object files to binary.')
	val_link_objects = [
		forge.P2K_DIR_EP1_OBJS / 'AutoRun.o',
		forge.P2K_DIR_EP1_OBJS / 'ElfLoader.o',
		forge.P2K_DIR_EP1_OBJS / 'ElfLoaderApp.o',
		forge.P2K_DIR_EP1_OBJS / 'LibC.o',
		val_system_info_o,
		val_combined_sym
	]
	val_elfpack_elf = arg_output / 'ElfPack.elf'
	val_elfpack_bin = arg_output / 'ElfPack.bin'
	val_elfpack_sym = arg_output / 'ElfPack.sym'
	forge.ep1_ads_armlink(val_link_objects, val_elfpack_elf, arg_address, val_elfpack_sym)
	forge.ep1_ads_fromelf(val_elfpack_elf, val_elfpack_bin)
	logging.info(f'')

	logging.info(f'Creating Flash&Backup 3 patches.')
	val_register_fpa = arg_output / 'Register.fpa'
	val_elfpack_fpa = arg_output / 'ElfPack.fpa'
	val_result_fpa = arg_output / 'Result.fpa'
	forge.bin2fpa(arg_fw, 'Andy51', 'ElfPack v1.0', arg_offset, val_elfpack_bin, val_elfpack_fpa, arg_firmware)
	generate_register_patch(
		arg_fw, 'Andy51', 'ElfPack v1.0 Register',
		val_elfpack_sym, val_register_sym, val_register_fpa, arg_firmware
	)
	forge.unite_fpa_patches(
		arg_fw, 'Andy51', 'Combined ElfPack v1.0 patch',
		[val_register_fpa, val_elfpack_fpa], val_result_fpa
	)
	logging.info(f'')

	logging.info(f'Creating ElfPack v1.0 library for Phone.')
	val_library_sym = arg_output / 'Lib.sym'
	val_library_asm = arg_output / 'Lib.asm'
	val_elfloader_lib = arg_output / 'elfloader.lib'
	generate_lib_sym(
		val_combined_sym, val_elfpack_sym, val_library_sym,
		[FUNC_INJECTION, 'APP_CALC_MainRegister', '_region_table'],
		['Ldr', 'UtilLogStringData', 'namecmp', 'u_utoa', '_ll_cmpu']
	)
	library_model = []
	functions = forge.ep1_libgen_model(val_library_sym, library_model)
	forge.ep1_libgen_asm(val_library_asm, library_model)
	forge.ep1_libgen_library(val_elfloader_lib, library_model, functions)
	logging.info(f'')

	logging.info(f'Compiling ElfPack v1.0 library for SDK.')
	val_library_obj = arg_output / 'Lib.o'
	val_libstd_static_lib = arg_output / 'libstd.a'
	forge.ep1_ads_armasm(val_library_asm, val_library_obj)
	forge.ep1_ads_armar([val_library_obj], val_libstd_static_lib)
	logging.info(f'')

	logging.info(f'ElfPack v1.0 building report.')
	logging.info(f'')
	logging.info(f'Important files:')
	logging.info(f'\t{val_elfloader_lib}\t-\tCompiled library for {arg_phone}.')
	logging.info(f'\t{val_result_fpa}\t-\tGenerated ElfPack v1.0 combined patch for Flash&Backup 3.')
	logging.info(f'')
	logging.info(f'Developer files:')
	logging.info(f'\t{val_libstd_static_lib}\t-\tCompiled library for SDK.')
	logging.info(f'\t{val_library_sym}\t-\tGenerated library entities list.')
	logging.info(f'')

	return True


# Arguments parsing routines.
class ArgsParser(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')


def parse_arguments() -> Namespace:
	hlp = {
		'd': 'ElfPack v1.0 PortKit Tool by EXL, 15-Dec-2023',
		'c': 'clean output directory before processing',
		'r': 'resolve precached iRAM function addresses',
		's': 'start address of CG0+CG1 firmware',
		'p': 'path to patterns file',
		'f': 'path to CG0+CG1 firmware file',
		'o': 'output artifacts directory',
		'v': 'verbose output'
	}
	epl = """examples:
	python ep1_portkit.py -c -r -s 0x10080000 -p ep1_func/General.pat -f E1_R373_G_0E.30.49R.smg -o ep1_build
	python ep1_portkit.py -c -r -s 0x10092000 -p ep1_func/General.pat -f L7_R4513_G_08.B7.ACR_RB.smg -o ep1_build
	python ep1_portkit.py -c -r -s 0x100A0000 -p ep1_func/General.pat -f V3i_R4441D_G_08.01.03R.smg -o ep1_build
	"""
	parser_args = ArgsParser(description=hlp['d'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-c', '--clean', required=False, action='store_true', help=hlp['c'])
	parser_args.add_argument('-r', '--ram-trans', required=False, action='store_true', help=hlp['r'])
	parser_args.add_argument('-s', '--start', required=True, type=forge.at_hex, metavar='OFFSET', help=hlp['s'])
	parser_args.add_argument('-p', '--patterns', required=True, type=forge.at_file, metavar='FILE.pat', help=hlp['p'])
	parser_args.add_argument('-f', '--firmware', required=True, type=forge.at_fw, metavar='FILE.smg', help=hlp['f'])
	parser_args.add_argument('-o', '--output', required=True, type=forge.at_dir, metavar='DIRECTORY', help=hlp['o'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_args()


def main() -> None:
	start_time = datetime.now()
	args = parse_arguments()

	logging.basicConfig(
		level=logging.DEBUG if args.verbose else logging.INFO,
		format='%(asctime)s %(levelname)s: %(message)s',
		datefmt='%d-%b-%Y %H:%M:%S'
	)

	if args.clean:
		forge.delete_all_files_in_directory(args.output)

	start_port_kit_work(args)

	time_elapsed = forge.format_timedelta(datetime.now() - start_time)
	logging.info(f'Time elapsed: "{time_elapsed}".')


if __name__ == '__main__':
	main()
