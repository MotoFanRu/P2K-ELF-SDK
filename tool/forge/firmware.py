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
	segments: list[str] = firmware.split('.')
	fw_major: str = firmware.replace('.' + segments[-1], '')
	fw_minor: str = segments[-1]
	return fw_major, fw_minor


def parse_phone_firmware(phone_firmware_filename: str) -> tuple[str, str]:
	segments_a: list[str] = phone_firmware_filename.split('_')        # First split string by '_'.
	segments_b: list[str] = phone_firmware_filename.split('.')        # Second split string by '.'.
	segments_a_ok: bool = len(segments_a) > 2                         # At least 3 segments in segments_a.
	segments_b_ok: bool = len(segments_b) > 2                         # At least 3 segments in segments_b.
	extension: str = segments_b[-1]                                   # Please use *.smg or *.bin extensions.
	extension_ok: bool = (extension == 'smg') or (extension == 'bin')

	if not (segments_a_ok and segments_b_ok and extension_ok):
		raise ValueError('wrong phone-firmware name format! Please use this pattern: E1_R373_G_0E.30.49R.smg')

	phone_name: str = segments_a[0]  # First segment of first split will be a phone name.
	firmware_name: str = phone_firmware_filename.replace(f'{phone_name}_', '').replace(f'.{extension}', '')

	return phone_name, firmware_name


def get_file_size(path: Path) -> int:
	if path.is_file():
		size: int = path.stat().st_size
		if 0x100000 < size < 0x4000000:  # Between 1...64 MiB.
			return size
	raise ValueError(f'something wrong with "{path}" file size, it must between 1...64 MiB.')


def determine_soc(start_firmware_address: int) -> str:
	soc_map: dict[int, str] = {
		0x10080000: 'LTE',
		0x10092000: 'LTE2',
		0x100A0000: 'LTE2',
	}
	return soc_map.get(start_firmware_address, 'Unknown')
