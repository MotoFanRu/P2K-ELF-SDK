#!/usr/bin/env python

"""
genlib.py script by EXL, 26-Nov-2023

Generate libraries by symdefs files.
"""

import os
import sys
import subprocess

from datetime import date


P2K_ELF_SDK_PATH = os.path.join('..', '..', '..', '..')
LIBRARIES_PATH = os.path.join(P2K_ELF_SDK_PATH, 'lib')
POSTLINK_UTILITY_WINDOWS = os.path.join(P2K_ELF_SDK_PATH, 'tool', 'win', 'ep2', 'postlink.exe')
POSTLINK_UTILITY_LINUX = os.path.join(P2K_ELF_SDK_PATH, 'tool', 'lin', 'ep2', 'postlink')
POSTLINK_UTILITY = POSTLINK_UTILITY_WINDOWS if sys.platform.startswith('win') else POSTLINK_UTILITY_LINUX


def generate_library(phone, firmware, version):
	print(f'=> Generate library for {phone}, {firmware}, {version} version:')
	print()
	symdef = os.path.join(LIBRARIES_PATH, phone + '_' + firmware, 'library.sym')
	if os.path.exists(symdef):
		args = [
			POSTLINK_UTILITY,
			'-stdlib', symdef,
			'-def', os.path.join('ldrAPI.def'),
			'-fw', firmware,
			'-v', version,
			'-header', os.path.join(P2K_ELF_SDK_PATH, 'ep', 'sdk', 'consts.h')
		]
		result = subprocess.run(args).returncode
		command = ' '.join(args)
		print(f'Result of "{command}" command is "{result}".')
	else:
		print(f'File "{symdef}" is not exist!')
	print()


def generate_parameters(phone_firmware):
	phone = phone_firmware.split('_')[0]
	firmware = phone_firmware.replace(phone + '_', '')
	version = str(date.today().strftime('%d%m%y')) + '1'
	return (phone, firmware, version)


def generate_all_libraries():
	phone_firmwares = os.listdir(LIBRARIES_PATH)
	for phone_firmware in phone_firmwares:
		generate_library(*generate_parameters(phone_firmware))


if __name__ == '__main__':
	print('genlib.py script by EXL, 26-Nov-2023')
	print('Generate libraries by symdefs files.')
	print()

	argc = len(sys.argv)
	if argc == 1:
		generate_all_libraries()
	elif argc == 2:
		generate_library(*generate_parameters(sys.argv[1]))
	else:
		print('Usage:\n\t./genlib.py [PHONE_FIRMWARE]')
		print()
		print('Example:\n\t./genlib.py \n\t./genlib.py E1_R373_G_0E.30.49R')
		print()
