# forge/patterns.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import logging

from pathlib import Path

from .hexer import int2hex
from .filesystem import move_file
from .constants import P2K_TOOL_PAT
from .constants import P2K_DIR_TOOL
from .invoker import invoke_external_command_res


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
