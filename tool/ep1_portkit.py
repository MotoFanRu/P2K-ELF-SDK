#!/usr/bin/env python3

import argparse
import logging
import sys
from argparse import Namespace
from pathlib import Path
from forge import generate_source_with_const_chars
from forge import parse_phone_firmware
from forge import parse_minor_major_firmware
from forge import get_file_size
from forge import hex2int
from forge import arrange16
from forge import determine_soc
from forge import P2K_DIR_EP1_FUNC
from forge import find_functions_from_patterns
from forge import delete_all_files_in_directory
from forge import create_combined_sym_file
from forge import validate_sym_file
from forge import get_function_address_from_sym_file
from forge import append_pattern_to_file

REGISTER_FUNCTION_INJECTION = 'APP_SyncML_MainRegister'


########################################################################################################################
#                                                                                                                      #
#                                                                                                                      #
#                                                                                                                      #
########################################################################################################################

def generate_system_information_source(phone_firmware: str, soc: str, source_file: Path) -> bool:
	system_info = {}
	phone, firmware = parse_phone_firmware(phone_firmware)
	major, minor = parse_minor_major_firmware(firmware)
	system_info['n_phone'] = phone
	system_info['n_platform'] = soc
	system_info['n_majorfw'] = major
	system_info['n_minorfw'] = minor
	return generate_source_with_const_chars(source_file, system_info)


def generate_register_symbol_file(combined_sym: Path, cgs_path: Path, register_func: str, out_dir: Path) -> bool:
	pat = out_dir / 'register.pat'
	sym = out_dir / 'register.sym'
	address = get_function_address_from_sym_file(combined_sym, register_func)
	if address != 0x00000000:
		append_pattern_to_file(pat, 'Register', 'D', f'{address:08X}')
		find_functions_from_patterns(pat, cgs_path, 0x00000000, False, sym)
	return False


########################################################################################################################
#                                                                                                                      #
#                                                                                                                      #
#                                                                                                                      #
########################################################################################################################

class ArgsParser(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')


def arg_type_fw(firmware_filename: str) -> Path:
	try:
		parse_phone_firmware(firmware_filename)
		return arg_type_file(firmware_filename)
	except ValueError as value_error:
		raise argparse.ArgumentTypeError(value_error)


def arg_type_dir(dirname: str) -> Path:
	path = Path(dirname)
	if not path.exists():
		path.mkdir()
	if not path.exists() or not path.is_dir():
		raise argparse.ArgumentTypeError(f'{dirname} is not directory')
	return path


def arg_type_file(filename: str) -> Path:
	path = Path(filename)
	if not path.is_file() and not path.exists():
		raise argparse.ArgumentTypeError(f'{filename} not found')
	return path


def arg_type_hex(hex_value: str) -> int:
	try:
		return hex2int(hex_value)
	except ValueError as value_error:
		raise argparse.ArgumentTypeError(value_error)


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
	parser_args.add_argument('-s', '--start', required=True, type=arg_type_hex, metavar='OFFSET', help=hlp['s'])
	parser_args.add_argument('-p', '--patterns', required=True, type=arg_type_file, metavar='FILE.pat', help=hlp['p'])
	parser_args.add_argument('-f', '--firmware', required=True, type=arg_type_fw, metavar='FILE.smg', help=hlp['f'])
	parser_args.add_argument('-o', '--output', required=True, type=arg_type_dir, metavar='DIRECTORY', help=hlp['o'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_args()


########################################################################################################################
#                                                                                                                      #
#                                                                                                                      #
#                                                                                                                      #
########################################################################################################################

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
	address = args.start + arrange16(get_file_size(args.firmware))  # Start + Offset.
	soc = determine_soc(args.start)

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
	function_sym_file = output / 'function.sym'
	combined_sym_file = output / 'combined.sym'
	lte2_irom_sym_file = P2K_DIR_EP1_FUNC / 'lte2_irom.sym'
	system_info_file = output / 'SysInfo.c'

	logging.info(f'Find SoC related functions from patterns')
	if soc == 'LTE':
		find_functions_from_patterns(P2K_DIR_EP1_FUNC / 'lte1.pat', firmware, start, False, platform_sym_file)
	elif soc == 'LTE2':
		find_functions_from_patterns(P2K_DIR_EP1_FUNC / 'lte2.pat', firmware, start, False, platform_sym_file)
	else:
		function_sym_file = combined_sym_file
		logging.warning(f'Unknown SoC platform, will skip generating platform symbols file')

	logging.info(f'Find general functions from patterns')
	find_functions_from_patterns(patterns, firmware, start, ram_trans, function_sym_file)

	logging.info(f'Combine all functions into one symbols file')
	if soc == 'LTE':
		create_combined_sym_file([function_sym_file, platform_sym_file], combined_sym_file)
	elif soc == 'LTE2':
		create_combined_sym_file([function_sym_file, platform_sym_file, lte2_irom_sym_file], combined_sym_file)

	logging.info(f'Validate combined symbols file')
	if not validate_sym_file(combined_sym_file):
		return False
	else:
		logging.info(f'The "{combined_sym_file}" sym file is validated')

	logging.info(f'Generate register symbols file')
	generate_register_symbol_file(combined_sym_file, firmware, REGISTER_FUNCTION_INJECTION, output)

	logging.info(f'Generate system information C-source file')
	generate_system_information_source(firmware_name, soc, system_info_file)

	return True


def main() -> None:
	args = parse_arguments()

	logging.basicConfig(
		level=logging.DEBUG if args.verbose else logging.INFO,
		format='%(asctime)s %(levelname)s: %(message)s',
		datefmt='%d-%b-%Y %H:%M:%S'
	)

	if args.clean:
		delete_all_files_in_directory(args.output)

	start_portkit_work(args)


if __name__ == '__main__':
	main()
