# forge/firmware.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

from pathlib import Path


def parse_minor_major_firmware(firmware: str) -> tuple[str, str]:
	segments = firmware.split('.')
	fw_major = firmware.replace('.' + segments[-1], '')
	fw_minor = segments[-1]
	return fw_major, fw_minor


def parse_phone_firmware(phone_firmware_filename: str) -> tuple[str, str]:
	segments_a = phone_firmware_filename.split('_')  # First split string by '_'.
	segments_b = phone_firmware_filename.split('.')  # Second split string by '.'.
	segments_a_ok = len(segments_a) > 2              # At least 3 segments in segments_a.
	segments_b_ok = len(segments_b) > 2              # At least 3 segments in segments_b.
	extension = segments_b[-1]                       # Please use *.smg or *.bin extensions.
	extension_ok = (extension == 'smg') or (extension == 'bin')

	if not (segments_a_ok and segments_b_ok and extension_ok):
		raise ValueError('wrong phone-firmware name format! Please use this pattern: E1_R373_G_0E.30.49R.smg')

	phone_name = segments_a[0]  # First segment of first split will be a phone name.
	firmware_name = phone_firmware_filename.replace(f'{phone_name}_', '').replace(f'.{extension}', '')

	return phone_name, firmware_name


def get_file_size(path: Path) -> int:
	if path.is_file() and path.exists():
		size = path.stat().st_size
		if 0x400000 < size < 0x4000000:  # Between 4...64 MiB.
			return size
	raise ValueError(f'something wrong with "{path}" file size.')


def determine_soc(start_firmware_address: int) -> str:
	if start_firmware_address == 0x10080000:
		return 'LTE'
	elif start_firmware_address == 0x10092000:
		return 'LTE2'
	elif start_firmware_address == 0x100A0000:
		return 'LTE2'
	else:
		return 'Unknown'
