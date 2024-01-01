# forge/patterns.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

import logging

from pathlib import Path

from .hexer import int2hex
from .hexer import hex2int
from .types import LibraryModel
from .types import MemoryRegion
from .firmware import determine_memory_region
from .filesystem import move_file
from .filesystem import check_files_if_exists
from .filesystem import check_files_extensions
from .constants import P2K_TOOL_PAT
from .constants import P2K_DIR_TOOL
from .invoker import invoke_external_command_res
from .symbols import combine_sym_str
from .symbols import dump_sym_file_to_library_model
from .utilities import get_current_datetime_formatted
from .utilities import is_string_filled_by_character


def pat_find(pat_p: Path, cgs_p: Path, base_address: int, ram_trans: bool, out_p: Path) -> bool:
	args: list[str] = [
		str(P2K_TOOL_PAT),
		'-ram-trans' if ram_trans else '-no-ram-trans',
		str(cgs_p),
		str(pat_p),
		str(out_p),
		int2hex(base_address)
	]
	# invoked: bool = invoke_external_command_res([pat_p, cgs_p], args)
	# if invoked:
	# 	move_file(P2K_DIR_TOOL / 'functions.sym', out_p)
	# return invoked
	return invoke_external_command_res([pat_p, cgs_p], args)


def pat_append(pat_p: Path, name: str, mode: str, pattern: str) -> None:
	with pat_p.open(mode='a', newline='\r\n') as f_o:
		logging.info(f'Will write "{name} {mode} {pattern}" to "{pat_p}" pattern file.')
		f_o.write(f'{name} {mode} {pattern}')


def sym2pat(sym_p: Path, pat_p: Path, fw_p: Path, offset: int, size: int, irom: bool) -> bool:
	files_are_here: bool = check_files_if_exists([sym_p, fw_p])
	sym_extension_is_ok: bool = check_files_extensions([sym_p], ['sym'])
	fw_extension_is_ok: bool = check_files_extensions([fw_p], ['bin', 'smg'])
	if files_are_here and sym_extension_is_ok and fw_extension_is_ok:
		model: LibraryModel = dump_sym_file_to_library_model(sym_p, True)
		if model:
			with fw_p.open(mode='rb') as f_i, pat_p.open(mode='w', newline='\r\n') as f_o:
				f_o.write(f'# Patterns file was generated by "forge" library.\n')
				f_o.write(f'# Source Code: https://github.com/MotoFanRu/P2K-ELF-SDK\n')
				f_o.write(f'# File: {fw_p.name}\n')
				f_o.write(f'# Offset: {int2hex(offset)}, {offset}\n')
				f_o.write(f'# Size: {int2hex(size)}, {size}\n')
				f_o.write(f'# Timestamp: {get_current_datetime_formatted()}\n\n')
				for addr, mode, name in model:
					symbol: str = combine_sym_str(addr, mode, name)
					if mode != 'C':
						address: int = hex2int(addr)
						mem_reg: MemoryRegion = determine_memory_region(address)
						if (irom and mem_reg == MemoryRegion.IROM) or (not irom and mem_reg == MemoryRegion.ROM):
							if irom:
								f_i.seek(address)  # Offset is "0" here.
							else:
								f_i.seek(address - offset)
							hex_data_spaced: str = f_i.read(size).hex(' ').upper()
							hex_data: str = hex_data_spaced.replace(' ', '')
							if not hex_data_spaced or is_string_filled_by_character(hex_data, 'F'):
								desc: str = f'because FF-empty, probably Elf Loader API?'
								logging.warning(f'Skip {mem_reg.name} entry: "{symbol}", {desc}')
							else:
								logging.info(f'Write {mem_reg.name} entry: "{symbol}", pattern: {hex_data}.')
								f_o.write(f'# Entry: {symbol}\n')
								f_o.write(f'# Data: {hex_data_spaced}\n')
								f_o.write(f'{name} {mode} {hex_data}\n')
								f_o.write(f'\n')
						elif mem_reg == MemoryRegion.IRAM:
							desc: str = f'disable RAM-Trans while symbols file generation.'
							logging.warning(f'Skip {mem_reg.name} entry: "{symbol}", {desc}')
						else:
							logging.warning(f'Skip {mem_reg.name} entry: "{symbol}".')
					else:
						logging.warning(f'Skip CONST entry: "{symbol}".')
				return True
	return False
