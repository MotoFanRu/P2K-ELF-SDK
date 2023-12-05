# forge/pattern.py

from .const import *
from .invoke import invoke_external_command
from .file import move_file
from pathlib import Path


def find_functions_from_patterns(pat_p: Path, cgs_p: Path, base_address: int, ram_trans: bool, out_p: Path) -> bool:
	if pat_p.is_file() and cgs_p.is_file() and pat_p.exists() and cgs_p.exists():
		args = [
			str(P2K_TOOL_PAT),
			'-ram-trans' if ram_trans else '-no-ram-trans',
			str(cgs_p),
			str(pat_p),
			f'0x{base_address:08X}'
		]
		result = invoke_external_command(args)
		if result == 0:
			move_file(P2K_DIR_TOOL / 'functions.sym', out_p)
			return True
	return False
