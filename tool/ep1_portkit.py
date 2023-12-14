#!/usr/bin/env python3

import argparse
import logging
import sys
from argparse import Namespace
from datetime import datetime
from pathlib import Path

import forge

REGISTER_FUNCTION_INJECTION = 'APP_SyncML_MainRegister'
REGISTER_FUNCTION = 'Register'
AUTORUN_FUNCTION = 'AutorunMain'


#  Generators.
def generate_lib_sym(p_i_f: Path, p_i_e: Path, p_o_l: Path, names_skip: list[str], patterns_add: list[str]) -> bool:
	if p_i_f.is_file() and p_i_f.exists() and p_i_e.is_file() and p_i_e.exists():
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


def generate_register_patch(fw: str, author: str, desc: str, p_elf_sym: Path, p_reg_sym: Path, p_patch: Path) -> bool:
	if p_elf_sym.is_file() and p_elf_sym.exists() and p_reg_sym.is_file() and p_reg_sym.exists():
		hex_data = forge.int2hex_r(forge.get_function_address_from_sym_file(p_elf_sym, AUTORUN_FUNCTION) + 1)  # Thumb
		reg_address = forge.int2hex_r(forge.get_function_address_from_sym_file(p_reg_sym, REGISTER_FUNCTION))

		forge.generate_fpa(fw, author, desc, reg_address, hex_data, p_patch)
		return True
	else:
		logging.error(f'Cannot open symbol files: "{p_elf_sym}" and "{p_reg_sym}".')
	return False


def generate_system_information_source(phone_firmware: str, soc: str, source_file: Path) -> bool:
	system_info = {}
	phone, firmware = forge.parse_phone_firmware(phone_firmware)
	major, minor = forge.parse_minor_major_firmware(firmware)
	system_info['n_phone'] = phone
	system_info['n_platform'] = soc
	system_info['n_majorfw'] = major
	system_info['n_minorfw'] = minor
	return forge.generate_source_with_const_chars(source_file, system_info)


def generate_register_symbol_file(combined_sym: Path, cgs_path: Path, register_func: str, out_dir: Path) -> bool:
	pat = out_dir / 'register.pat'
	sym = out_dir / 'register.sym'
	address = forge.get_function_address_from_sym_file(combined_sym, register_func)
	if address != 0x00000000:
		forge.append_pattern_to_file(pat, 'Register', 'D', forge.int2hex_r(address))
		forge.find_functions_from_patterns(pat, cgs_path, 0x00000000, False, sym)
	return False


#  Portkit working flow.
def start_portkit_work(args: Namespace) -> bool:
	logging.info(f'Start working with arguments:')
	logging.info(f'\tverbose={args.verbose}')
	logging.info(f'\tclean={args.clean}')
	logging.info(f'')

	output = args.output
	patterns = args.patterns
	firmware = args.firmware
	firmware_name = args.firmware.name
	start = args.start
	ram_trans = args.ram_trans
	address = args.start + forge.arrange16(forge.get_file_size(args.firmware))  # Start + Offset.
	soc = forge.determine_soc(args.start)

	logging.info(f'Values:')
	logging.info(f'\tram_trans={ram_trans}')
	logging.info(f'\tstart=0x{start:08X}')
	logging.info(f'\tpatterns={patterns}')
	logging.info(f'\tfirmware={firmware}')
	logging.info(f'\tfirmware_name={firmware_name}')
	logging.info(f'\toutput={output}')
	logging.info(f'\taddress=0x{address:08X}')
	logging.info(f'\tsoc={soc}')
	logging.info(f'')

	platform_sym_file = output / 'platform.sym'
	function_sym_file = output / 'functions.sym'
	combined_sym_file = output / 'combined.sym'
	lte1_patterns_file = forge.P2K_DIR_EP1_FUNC / 'lte1.pat'
	lte2_patterns_file = forge.P2K_DIR_EP1_FUNC / 'lte2.pat'
	lte2_irom_sym_file = forge.P2K_DIR_EP1_FUNC / 'lte2_irom.sym'
	system_info_file_c = output / 'SysInfo.c'
	system_info_file_o = output / 'SysInfo.o'

	logging.info(f'Finding SoC related functions from patterns.')
	if soc == 'LTE':
		forge.find_functions_from_patterns(lte1_patterns_file, firmware, start, False, platform_sym_file)
	elif soc == 'LTE2':
		forge.find_functions_from_patterns(lte2_patterns_file, firmware, start, False, platform_sym_file)
	else:
		function_sym_file = combined_sym_file
		logging.warning(f'Unknown SoC platform, will skip generating platform symbols file.')

	logging.info(f'Finding general functions from patterns.')
	forge.find_functions_from_patterns(patterns, firmware, start, ram_trans, function_sym_file)

	logging.info(f'Combining all functions into one symbols file.')
	if soc == 'LTE':
		forge.create_combined_sym_file([function_sym_file, platform_sym_file], combined_sym_file)
	elif soc == 'LTE2':
		forge.create_combined_sym_file([function_sym_file, platform_sym_file, lte2_irom_sym_file], combined_sym_file)

	logging.info(f'Validating combined symbols file.')
	if not forge.validate_sym_file(combined_sym_file):
		return False
	else:
		logging.info(f'The "{combined_sym_file}" sym file is validated.')

	logging.info(f'Generating register symbols file.')
	generate_register_symbol_file(combined_sym_file, firmware, REGISTER_FUNCTION_INJECTION, output)

	logging.info(f'Generating system information C-source file.')
	generate_system_information_source(firmware_name, soc, system_info_file_c)

	logging.info(f'Compiling system C-source files.')
	forge.compile_c_ep1_ads_tcc(system_info_file_c, system_info_file_o)

	logging.info(f'Linking object files to binary.')
	p_o = [
		forge.P2K_DIR_EP1_OBJS / 'AutoRun.o',
		forge.P2K_DIR_EP1_OBJS / 'ElfLoader.o',
		forge.P2K_DIR_EP1_OBJS / 'ElfLoaderApp.o',
		forge.P2K_DIR_EP1_OBJS / 'LibC.o',
		system_info_file_o,
		combined_sym_file
	]
	p_e = output / 'ElfPack.elf'
	p_b = output / 'ElfPack.bin'
	p_s = output / 'ElfPack.sym'
	forge.link_o_ep1_ads_armlink(p_o, p_e, address, p_s)
	forge.bin_elf_ep1_ads_fromelf(p_e, p_b)

	logging.info(f'Creating Flash & Backup 3 patches.')
	p_r = output / 'register.sym'
	p_f = output / 'Register.fpa'
	p_c = output / 'ElfPack.fpa'
	phone, fw = forge.parse_phone_firmware(firmware_name)
	forge.bin2fpa(fw, 'Andy51', 'ElfPack v1.0', address, p_b, p_c)
	generate_register_patch(fw, 'Andy51', 'Register ElfPack v1.0', p_s, p_r, p_f)

	logging.info(f'Creating ElfPack v1.0 library for Phone.')
	p_l = output / 'Lib.sym'
	p_a = output / 'Lib.asm'
	p_p = output / 'elfloader.lib'
	generate_lib_sym(
		combined_sym_file, p_s, p_l,
		[REGISTER_FUNCTION_INJECTION, 'APP_CALC_MainRegister', '_region_table'],
		['Ldr', 'UtilLogStringData', 'namecmp', 'u_utoa', '_ll_cmpu']
	)
	library_model = []
	functions = forge.libgen_ep1_fill_library_model(p_l, library_model)
	forge.libgen_ep1_create_assembler_source(p_a, library_model)
	forge.libgen_ep1_create_library(p_p, library_model, functions)

	logging.info(f'Compiling ElfPack v1.0 library for SDK.')
	p_d = output / 'Lib.o'
	p_t = output / 'libstd.a'
	forge.assembly_asm_ep1_ads_armasm(p_a, p_d)
	forge.packing_static_lib_ep1_ads_armar([p_d], p_t)

	logging.info(f'ElfPack v1.0 report.')
	logging.info(f'')
	logging.info(f'Important files:')
	logging.info(f'\t{p_t}\t-\tCompiled library for SDK.')
	logging.info(f'\t{p_p}\t-\tCompiled library for phone.')
	logging.info(f'\t{p_l}\t-\tGenerated library entities list.')
	logging.info(f'\t{p_c}\t-\tGenerated ElfPack v1.0 patch for Flash & Backup 3.')
	logging.info(f'\t{p_f}\t-\tGenerated ElfPack v1.0 register patch for Flash & Backup 3.')
	logging.info(f'')

	return True


#  Arguments parsing routines.
class ArgsParser(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')


def parse_arguments() -> Namespace:
	hlp = {
		'd': 'ElfPack v1.0 PortKit Tool by EXL, 05-Dec-2023',
		'c': 'clean output directory before processing',
		'r': 'resolve precached iRAM function addresses',
		's': 'start address of CG0+CG1 firmware',
		'p': 'path to patterns file',
		'f': 'path to CG0+CG1 firmware file',
		'o': 'output artifacts directory',
		'v': 'verbose output'
	}
	epl = """examples:
	python ep1_portkit.py -c -r -s 0x10080000 -p ep1_func/general.pat -f E1_R373_G_0E.30.49R.smg -o ep1_build
	python ep1_portkit.py -c -r -s 0x10092000 -p ep1_func/general.pat -f L7_R4513_G_08.B7.ACR_RB.smg -o ep1_build
	python ep1_portkit.py -c -r -v -s 0x100A0000 -p ep1_func/general.pat -f V3i_R4441D_G_08.01.03R.smg -o ep1_build
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

	start_portkit_work(args)

	time_elapsed = forge.format_timedelta(datetime.now() - start_time)
	logging.info(f'Time elapsed: "{time_elapsed}".')


if __name__ == '__main__':
	main()
