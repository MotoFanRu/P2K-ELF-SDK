#!/usr/bin/env python3

import argparse
import logging
import sys
from argparse import Namespace
from pathlib import Path
from forge import parse_phone_firmware
from forge import hex2int


def delete_all_files_in_output(args: Namespace) -> None:
	output_directory = args.output
	objects = output_directory.iterdir()
	files_to_clean = []
	for obj in objects:
		if obj.is_file():
			files_to_clean.append(obj)
	if len(files_to_clean) > 0:
		logging.info(f'Clean all files in "{output_directory}" directory.')
		for file_path in files_to_clean:
			if file_path.is_file():
				logging.info(f'Delete "{file_path}" file.')
				file_path.unlink()


########################################################################################################################
#                                                                                                                      #
#                                                                                                                      #
#                                                                                                                      #
########################################################################################################################

class ArgsParser(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')


def arg_type_firmware(firmware_filename: str) -> list[str]:
	try:
		return parse_phone_firmware(firmware_filename)
	except ValueError as value_error:
		raise argparse.ArgumentTypeError(value_error)


def arg_type_dir(dirname: str) -> Path:
	path = Path(dirname)
	if not path.exists():
		path.mkdir()
	if not path.is_dir():
		raise argparse.ArgumentTypeError(f'{dirname} is not directory')
	return path


def arg_type_file(filename: str) -> Path:
	path = Path(filename)
	if not path.is_file():
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
	parser_args.add_argument('-f', '--firmware', required=True, type=arg_type_file, metavar='FILE.smg', help=hlp['f'])
	parser_args.add_argument('-o', '--output', required=True, type=arg_type_dir, metavar='DIRECTORY', help=hlp['o'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_args()


########################################################################################################################
#                                                                                                                      #
#                                                                                                                      #
#                                                                                                                      #
########################################################################################################################

def main() -> None:
	args = parse_arguments()

	logging.basicConfig(
		level=logging.DEBUG if args.verbose else logging.INFO,
		format='%(asctime)s - %(name)s:%(levelname)s: %(message)s',
		datefmt='%d-%b-%Y %H:%M:%S'
	)
	logging.info(f'Start working with arguments:')
	logging.info(f'\tverbose={args.verbose}')
	logging.info(f'\tclean={args.clean}')
	logging.info(f'\tram_trans={args.ram_trans}')
	logging.info(f'\tstart=0x{args.start:08X}')
	logging.info(f'\tpatterns={args.patterns}')
	logging.info(f'\tfirmware={args.firmware}')
	logging.info(f'\toutput={args.output}')

	if args.clean:
		delete_all_files_in_output(args)


if __name__ == '__main__':
	main()
