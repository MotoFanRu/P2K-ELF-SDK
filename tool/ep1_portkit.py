#!/usr/bin/env python3


import argparse
import forge
from pathlib import Path


def arg_type_firmware(firmware):
	path = Path(firmware)
	phone_firmware = path.name.split('_')

def arg_type_dir(dirname):
	path = Path(dirname)
	if path.exists() and path.is_dir():
		return path
	else:
		return path.mkdir()


def arg_type_file(filename):
	path = Path(filename)
	if path.is_file():
		return path
	else:
		raise FileNotFoundError(filename)


def arg_type_hex(value):
	try:
		return int(value, 16)
	except ValueError:
		raise argparse.ArgumentTypeError(f'{value} is not a valid hexadecimal value')


def parse_arguments():
	parser_args = argparse.ArgumentParser(description='ElfPack v1.0 PortKit Tool by EXL, 05-Dec-2023')
	parser_args.add_argument(
		'-c', '--clean', required=False, action='store_true', help='clean output directory before processing'
	)
	parser_args.add_argument(
		'-r', '--ram-trans', required=False, action='store_true', help='resolve precached iRAM function addresses'
	)
	parser_args.add_argument(
		'-s', '--start', required=True, type=arg_type_hex, metavar='OFFSET', help='start address of CG0+CG1 firmware'
	)
	parser_args.add_argument(
		'-p', '--patterns', required=True, type=arg_type_file, metavar='FILE.pat', help='path to patterns file'
	)
	parser_args.add_argument(
		'-f', '--firmware', required=True, type=arg_type_file, metavar='FILE.smg', help='path to CG0+CG1 firmware file'
	)
	parser_args.add_argument(
		'-o', '--output', required=True, type=arg_type_dir, metavar='DIRECTORY', help='output artifacts directory'
	)
	parser_args.add_argument(
		'-v', '--verbose', required=False, action='store_true', help='verbose and debug output'
	)
	return parser_args.parse_args()


def main():
	args = parse_arguments()
	print(args)


if __name__ == '__main__':
	main()
