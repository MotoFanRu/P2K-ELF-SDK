# forge/pattern.py

import logging
from pathlib import Path

from .const import P2K_TOOL_PAT
from .const import P2K_DIR_TOOL
from .file import move_file
from .hexer import int2hex
from .invoke import invoke_external_command
from .invoke import invoke_external_command_res


def find_functions_from_patterns(pat_p: Path, cgs_p: Path, base_address: int, ram_trans: bool, out_p: Path) -> bool:
	if pat_p.is_file() and cgs_p.is_file() and pat_p.exists() and cgs_p.exists():
		args = [
			str(P2K_TOOL_PAT),
			'-ram-trans' if ram_trans else '-no-ram-trans',
			str(cgs_p),
			str(pat_p),
			str(out_p),
			int2hex(base_address)
		]
		return invoke_external_command_res(args)
		# result = invoke_external_command(args)
		# if result == 0:
		# 	move_file(P2K_DIR_TOOL / 'functions.sym', out_p)
		# 	return True
	return False


def append_pattern_to_file(pat_p: Path, name: str, mode: str, pattern: str) -> None:
	with pat_p.open(mode='a', newline='\r\n') as f_o:
		logging.info(f'Will write "{name} {mode} {pattern}" to "{pat_p}" pattern file.')
		f_o.write(f'{name} {mode} {pattern}')
