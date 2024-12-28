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
from .constants import P2K_EP2_GCC_GCC
from .constants import P2K_EP2_GCC_OBJCOPY
from .constants import P2K_EP2_GCC_NM
from .constants import P2K_EP2_GCC_AR
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
	args: list[str] = [str(P2K_EP1_ADS_TCC), '-I' + str(P2K_DIR_EP_SDK), '-c', '-bigend', '-apcs', '/interwork']
	if optimization:
		args.append('-O2')
	if custom_flags:
		args.extend(invoke_custom_arguments(custom_flags))
	args.append(str(p_in))
	args.append('-o')
	args.append(str(p_out))
	return invoke_external_command_res([p_in], args)


def ep1_ads_armasm(p_in: Path, p_out: Path, arm_mode: bool = False, custom_flags: list[str] | None = None) -> bool:
	logging.info(f'Assembling "{p_in}" to "{p_out}"...')
	args: list[str] = [
		str(P2K_EP1_ADS_ARMASM),
		'-32' if arm_mode else '-16',
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


def check_is_one_sym_here(p_i: list[Path]) -> bool:
	sym_files_counter: int = 0
	for path in p_i:
		if path.name.endswith('.sym'):
			sym_files_counter += 1
	if sym_files_counter > 1:
		logging.error('Too many *.sym files for linking.')
		return False
	return True


def ep1_ads_armlink(p_i: list[Path], p_o: Path, addr: int | None = None, p_o_sym: Path | None = None) -> bool:
	if not check_is_one_sym_here(p_i):
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


def ep1_ads_armlink_scatter(
	p_i: list[Path],
	p_o: Path,
	scatter: Path,
	viafile: Path | None = None,
	p_o_sym: Path | None = None
) -> bool:
	if not check_is_one_sym_here(p_i):
		return False
	if not check_files_if_exists([scatter, *p_i]):
		return False

	args: list[str] = [str(P2K_EP1_ADS_ARMLINK), '-scatter', str(scatter)]
	if viafile:
		args.append('-via')
		args.append(str(viafile))
	if p_o_sym:
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


def ep2_gcc_gcc(
	p_in: Path, p_out: Path,
	optimization: bool = False,
	custom_flags: list[str] | None = None,
	argon: bool = False
) -> bool:
	logging.info(f'Compiling "{p_in}" to "{p_out}"...')
	args: list[str] = [str(P2K_EP2_GCC_GCC), '-I' + str(P2K_DIR_EP_SDK), '-c']
	args.extend(['-mbig-endian', '-mthumb', '-mthumb-interwork'])
	if argon:
		args.extend(['-mbe32', '-march=armv6j', '-mtune=arm1136jf-s'])
	else:
		args.extend(['-march=armv4t', '-mtune=arm7tdmi-s'])
	args.extend(['-ffreestanding', '-fshort-wchar', '-fshort-enums', '-fpack-struct=4', '-fno-builtin'])
	if optimization:
		args.append('-O2')
	if custom_flags:
		args.extend(invoke_custom_arguments(custom_flags))
	args.append(str(p_in))
	args.append('-o')
	args.append(str(p_out))
	return invoke_external_command_res([p_in], args)


def ep2_gcc_link(
	p_i: list[Path], p_o: Path,
	optimization: bool = False,
	ld_script: Path = None,
	custom_flags: list[str] | None = None,
	argon: bool = False
) -> bool:
	args: list[str] = [str(P2K_EP2_GCC_GCC), '-I' + str(P2K_DIR_EP_SDK)]
	args.extend(['-mbig-endian', '-mthumb', '-mthumb-interwork'])
	if argon:
		args.extend(['-mbe32', '-march=armv6j', '-mtune=arm1136jf-s'])
	else:
		args.extend(['-march=armv4t', '-mtune=arm7tdmi-s'])
	args.extend(['-ffreestanding', '-fshort-wchar', '-fshort-enums', '-fpack-struct=4', '-fno-builtin'])
	if optimization:
		args.append('-O2')
	if check_files_if_exists([ld_script], False):
		args.append(f'-T{str(ld_script)}')
	args.extend(['-nostdlib', '-Wl,--gc-sections'])
	if custom_flags:
		args.extend(invoke_custom_arguments(custom_flags))
	for path in p_i:
		args.append(str(path))
	args.append('-o')
	args.append(str(p_o))
	return invoke_external_command_res(p_i, args)


def ep2_gcc_objcopy(p_in: Path, p_out: Path) -> bool:
	args: list[str] = [
		str(P2K_EP2_GCC_OBJCOPY),
		'-O',
		'binary',
		'-j',
		'.text*',
		str(p_in),
		str(p_out)
	]
	return invoke_external_command_res([p_in], args)


def ep2_gcc_nm(p_in: Path, p_out: Path) -> bool:
	args: list[str] = [ str(P2K_EP2_GCC_NM), str(p_in) ]
	return invoke_external_command_res([p_in], args, p_out)


def ep2_gcc_ar(p_in: list[Path], p_out: Path) -> bool:
	logging.info(f'Packing "{p_out}" static library...')
	args: list[str] = [
		str(P2K_EP2_GCC_AR),
		'rcs',
		str(p_out)
	]
	for obj in p_in:
		args.append(str(obj))
	return invoke_external_command_res(p_in, args)


def toolchain_compile(
	p_in: Path, p_out: Path,
	optimization: bool = False,
	custom_flags: list[str] | None = None,
	gcc: bool = False, argon: bool = False
) -> bool:
	if gcc:
		return ep2_gcc_gcc(p_in, p_out, optimization, custom_flags, argon)
	else:
		return ep1_ads_tcc(p_in, p_out, optimization, custom_flags)
