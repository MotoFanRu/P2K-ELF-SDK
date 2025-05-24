# forge/constants.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

import sys

from pathlib import Path

from .filesystem import check_files_if_exists


def is_win() -> bool:
	return sys.platform.startswith('win')


def e_app(executable_name: str) -> str:
	return (executable_name + '.exe') if is_win() else executable_name


def e_bin_in_path(executable_path: Path) -> str:
	return executable_path if check_files_if_exists([executable_path], False) else executable_path.name


P2K_TOOLCHAIN_ROOT: Path = Path(__file__).resolve().parent.parent.parent.parent

P2K_DIR_LIB: Path = P2K_TOOLCHAIN_ROOT / 'res'
P2K_DIR_EP_SDK: Path = P2K_TOOLCHAIN_ROOT / 'sdk'
P2K_DIR_EP1_INC: Path = P2K_TOOLCHAIN_ROOT / 'ep1' / 'inc'
P2K_DIR_EP1_LIB: Path = P2K_TOOLCHAIN_ROOT / 'ep1' / 'lib'
P2K_DIR_EP2_INC: Path = P2K_TOOLCHAIN_ROOT / 'ep2' / 'inc'
P2K_DIR_EP2_LIB: Path = P2K_TOOLCHAIN_ROOT / 'ep2' / 'lib'
P2K_DIR_TOOL: Path = P2K_TOOLCHAIN_ROOT / 'tool'
P2K_DIR_TOOL_BIN: Path = P2K_DIR_TOOL / 'bin'
P2K_DIR_TOOL_KITCHEN: Path = P2K_DIR_TOOL / 'kitchen'
P2K_DIR_CG: Path = P2K_DIR_TOOL / 'cg'
P2K_DIR_COMPILER: Path = P2K_DIR_TOOL / 'compiler'
P2K_DIR_EP1_ADS: Path = P2K_DIR_COMPILER / ('ARM-ADS-WIN' if is_win() else 'ARM-ADS-LIN')
P2K_DIR_EP1_ADS_BIN: Path = P2K_DIR_EP1_ADS / 'bin'
P2K_DIR_EP2_GCC: Path = P2K_DIR_COMPILER / ('ARM-GCC-WIN' if is_win() else 'ARM-GCC-LIN')
P2K_DIR_EP2_GCC_BIN: Path = P2K_DIR_EP2_GCC / 'bin'
P2K_DIR_EP1: Path = P2K_TOOLCHAIN_ROOT / 'ep1'
P2K_DIR_EP1_DEF: Path = P2K_DIR_EP1 / 'def'
P2K_DIR_EP1_PAT: Path = P2K_DIR_EP1 / 'pts'
P2K_DIR_EP1_OBJ: Path = P2K_DIR_EP1 / 'obj'
P2K_DIR_EP1_SRC: Path = P2K_DIR_EP1 / 'src'
P2K_DIR_EP1_SYM: Path = P2K_DIR_EP1 / 'sym'
P2K_DIR_EP1_TPL: Path = P2K_DIR_EP1 / 'tpl'
P2K_DIR_EP1_OBJ_OLD: Path = P2K_DIR_EP1_OBJ / 'old'
P2K_DIR_EP2: Path = P2K_TOOLCHAIN_ROOT / 'ep2'
P2K_DIR_EP2_DEF: Path = P2K_DIR_EP2 / 'def'
P2K_DIR_EP2_SRC: Path = P2K_DIR_EP2 / 'src'
P2K_DIR_EP2_TPL: Path = P2K_DIR_EP2 / 'tpl'
P2K_DIR_EP2_FPA: Path = P2K_DIR_EP2 / 'fpa'

P2K_TOOL_PAT: Path = e_bin_in_path(P2K_DIR_TOOL_BIN / e_app('pat'))
P2K_TOOL_POSTLINK: Path = e_bin_in_path(P2K_DIR_TOOL_BIN / e_app('postlink'))

P2K_EP1_ADS_TCC: Path = e_bin_in_path(P2K_DIR_EP1_ADS_BIN / e_app('tcc'))
P2K_EP1_ADS_TCPP: Path = e_bin_in_path(P2K_DIR_EP1_ADS_BIN / e_app('tcpp'))
P2K_EP1_ADS_ARMCC: Path = e_bin_in_path(P2K_DIR_EP1_ADS_BIN / e_app('armcc'))
P2K_EP1_ADS_ARMCPP: Path = e_bin_in_path(P2K_DIR_EP1_ADS_BIN / e_app('armcpp'))
P2K_EP1_ADS_ARMAR: Path = e_bin_in_path(P2K_DIR_EP1_ADS_BIN / e_app('armar'))
P2K_EP1_ADS_ARMASM: Path = e_bin_in_path(P2K_DIR_EP1_ADS_BIN / e_app('armasm'))
P2K_EP1_ADS_ARMLINK: Path = e_bin_in_path(P2K_DIR_EP1_ADS_BIN / e_app('armlink'))
P2K_EP1_ADS_FROMELF: Path = e_bin_in_path(P2K_DIR_EP1_ADS_BIN / e_app('fromelf'))

P2K_EP2_GCC_AR: Path = e_bin_in_path(P2K_DIR_EP2_GCC_BIN / e_app('arm-none-eabi-ar'))
P2K_EP2_GCC_AS: Path = e_bin_in_path(P2K_DIR_EP2_GCC_BIN / e_app('arm-none-eabi-as'))
P2K_EP2_GCC_GCC: Path = e_bin_in_path(P2K_DIR_EP2_GCC_BIN / e_app('arm-none-eabi-gcc'))
P2K_EP2_GCC_GPP: Path = e_bin_in_path(P2K_DIR_EP2_GCC_BIN / e_app('arm-none-eabi-g++'))
P2K_EP2_GCC_LD: Path = e_bin_in_path(P2K_DIR_EP2_GCC_BIN / e_app('arm-none-eabi-ld'))
P2K_EP2_GCC_NM: Path = e_bin_in_path(P2K_DIR_EP2_GCC_BIN / e_app('arm-none-eabi-nm'))
P2K_EP2_GCC_OBJCOPY: Path = e_bin_in_path(P2K_DIR_EP2_GCC_BIN / e_app('arm-none-eabi-objcopy'))
P2K_EP2_GCC_OBJDUMP: Path = e_bin_in_path(P2K_DIR_EP2_GCC_BIN / e_app('arm-none-eabi-objdump'))
P2K_EP2_GCC_RANLIB: Path = e_bin_in_path(P2K_DIR_EP2_GCC_BIN / e_app('arm-none-eabi-ranlib'))
P2K_EP2_GCC_STRIP: Path = e_bin_in_path(P2K_DIR_EP2_GCC_BIN / e_app('arm-none-eabi-strip'))

P2K_SDK_CONSTS_H: Path = P2K_DIR_EP_SDK / 'consts.h'
P2K_EP1_API_DEF: Path = P2K_DIR_EP1_DEF / 'ElfLoaderAPI1.def'
P2K_EP2_API_DEF: Path = P2K_DIR_EP2_DEF / 'ElfLoaderAPI2.def'
P2K_EP2_NMS_DEF: Path = P2K_DIR_EP2_DEF / 'EntriesNames.def'

ADS_SYM_FILE_HEADER: str = '#<SYMDEFS>#symdef-file'

MAX_BINARY_CHUNK_READ: int = 4096

# Motorola phones based on Argon+, ArgonLV, ArgonLVLT, and similar SoCs.
P2K_ARGON_PHONES: list[str] = ['V3xx', 'V6', 'K3', 'K3m', 'Z9', 'V9', 'M702iG', 'M702iS']
