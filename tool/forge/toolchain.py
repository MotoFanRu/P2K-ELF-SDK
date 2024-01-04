# forge/toolchain.py
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
from .constants import P2K_DIR_EP_SDK
from .constants import P2K_EP1_ADS_TCC
from .constants import P2K_EP1_ADS_ARMLINK
from .constants import P2K_EP1_ADS_FROMELF
from .constants import P2K_EP1_ADS_ARMASM
from .constants import P2K_EP1_ADS_ARMAR
from .invoker import invoke_external_command_res
from .invoker import invoke_custom_arguments
from .filesystem import check_files_if_exists


def gen_src_const_chars(header_file: Path, array_dict: dict[str, str]) -> bool:
	try:
		with header_file.open(mode='w', newline='\r\n') as f_o:
			for key, value in array_dict.items():
				length: int = len(value) + 1  # '\0'
				template: str = f'const char {key}[{length}]\t= "{value}";'
				logging.debug(template)
				f_o.write(template)
				f_o.write('\n')
		return check_files_if_exists([header_file])
	except OSError as error:
		logging.error(f'Cannot write into "{header_file}", error: {error}')
		return False


def ep1_ads_tcc(p_in: Path, p_out: Path, optimization: bool = False, custom_flags: list[str] | None = None) -> bool:
	logging.info(f'Compiling "{p_in}" to "{p_out}"...')
	args: list[str] = []
	args.append(str(P2K_EP1_ADS_TCC))
	args.append('-I' + str(P2K_DIR_EP_SDK))
	args.append('-c')
	args.append('-bigend')
	args.append('-apcs')
	args.append('/interwork')
	if optimization:
		args.append('-O2')
	if custom_flags:
		args.extend(invoke_custom_arguments(custom_flags))
	args.append(str(p_in))
	args.append('-o')
	args.append(str(p_out))
	return invoke_external_command_res([p_in], args)


def ep1_ads_armasm(p_in: Path, p_out: Path, custom_flags: list[str] | None = None) -> bool:
	logging.info(f'Assembling "{p_in}" to "{p_out}"...')
	args: list[str] = [
		str(P2K_EP1_ADS_ARMASM),
		'-16',
		'-bigend',
		'-apcs',
		'/interwork',
		*invoke_custom_arguments(custom_flags),
		str(p_in),
		'-o',
		str(p_out)
	]
	return invoke_external_command_res([p_in], args)


def ep1_ads_armar(p_in: list[Path], p_out: Path, custom_flags: list[str] | None = None) -> bool:
	logging.info(f'Packing "{p_out}" static library...')
	args: list[str] = [
		str(P2K_EP1_ADS_ARMAR),
		*invoke_custom_arguments(custom_flags),
		'--create',
		'-cr',
		str(p_out)
	]
	for obj in p_in:
		args.append(str(obj))
	return invoke_external_command_res(p_in, args)


def ep1_ads_armlink(p_i: list[Path], p_o: Path, addr: int | None = None, p_o_sym: Path | None = None) -> bool:
	sym_files_counter: int = 0
	for path in p_i:
		if path.name.endswith('.sym'):
			sym_files_counter += 1
	if sym_files_counter > 1:
		logging.error('Too many *.sym files for linking.')
		return False

	args: list[str] = [str(P2K_EP1_ADS_ARMLINK)]
	if addr is not None:
		args.append('-ro-base')
		args.append(int2hex(addr))
	if p_o_sym is not None:
		args.append('-symdefs')
		args.append(str(p_o_sym))
	for path in p_i:
		args.append(str(path))
	args.append('-o')
	args.append(str(p_o))
	return invoke_external_command_res(p_i, args)


def ep1_ads_fromelf(p_in: Path, p_out: Path) -> bool:
	args: list[str] = [
		str(P2K_EP1_ADS_FROMELF),
		str(p_in),
		'-bin',
		'-output',
		str(p_out)
	]
	return invoke_external_command_res([p_in], args)
