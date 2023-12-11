# forge/compiler.py

import logging
from pathlib import Path
from .const import *
from .invoke import invoke_external_command
from .hexer import int2hex


def generate_source_with_const_chars(header_file: Path, array_dict: dict[str, str]) -> bool:
	try:
		with header_file.open(mode='w', newline='\r\n') as f_o:
			for key, value in array_dict.items():
				length = len(value) + 1  # '\0'
				template = f'const char {key}[{length}]\t= "{value}";'
				logging.debug(template)
				f_o.write(template)
				f_o.write('\n')
		return True
	except FileNotFoundError as error:
		logging.error(error)
		return False


def compile_c_ep1_ads_tcc(p_in: Path, p_out: Path, custom_flags: list[str] | None = None) -> bool:
	if custom_flags is None:
		custom_flags = []
	if p_in.is_file() and p_in.exists():
		logging.info(f'Compiling "{p_in}" to "{p_out}"...')
		args = [
			str(P2K_EP1_ADS_TCC),
			'-I' + str(P2K_DIR_EP_SDK),
			'-c',
			'-bigend',
			'-apcs',
			'/interwork',
			*custom_flags,
			str(p_in),
			'-o',
			str(p_out)
		]
		result = invoke_external_command(args)
		if result == 0:
			return True
	return False


def link_o_ep1_ads_armlink(p_i: list[Path], p_o: Path, addr: int | None = None, p_o_sym: Path | None = None) -> bool:
	sym_files_counter = 0
	for p in p_i:
		if not p.is_file() or not p.exists():
			logging.error(f'File {p} is not exist')
			return False
		if p.name.endswith('.sym'):
			sym_files_counter += 1

	if sym_files_counter > 1:
		logging.error(f'Too many *.sym files')
		return False

	args = [str(P2K_EP1_ADS_ARMLINK)]
	if addr is not None:
		args.append('-ro-base')
		args.append(int2hex(addr))
	if p_o_sym is not None:
		args.append('-symdefs')
		args.append(str(p_o_sym))
	for p in p_i:
		args.append(str(p))
	args.append('-o')
	args.append(str(p_o))
	return True if (invoke_external_command(args) == 0) else False


def bin_elf_ep1_ads_fromelf(p_in: Path, p_out: Path) -> bool:
	if p_in.is_file() and p_in.exists():
		args = [
			str(P2K_EP1_ADS_FROMELF),
			str(p_in),
			'-bin',
			'-output',
			str(p_out)
		]
		result = invoke_external_command(args)
		if result == 0:
			return True
	return False
