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


def is_win() -> bool:
	return sys.platform.startswith('win')


def e_app(executable_name: str) -> str:
	return (executable_name + '.exe') if is_win() else executable_name


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
P2K_DIR_EP1_ADS: Path = P2K_DIR_COMPILER / ('ep1_win_ADS' if is_win() else 'ep1_lin_ADS')
P2K_DIR_EP1_ADS_BIN: Path = P2K_DIR_EP1_ADS / 'bin'
P2K_DIR_EP1: Path = P2K_TOOLCHAIN_ROOT / 'ep1'
P2K_DIR_EP1_DEF: Path = P2K_DIR_EP1 / 'def'
P2K_DIR_EP1_PAT: Path = P2K_DIR_EP1 / 'pat'
P2K_DIR_EP1_OBJ: Path = P2K_DIR_EP1 / 'obj'
P2K_DIR_EP1_SRC: Path = P2K_DIR_EP1 / 'src'
P2K_DIR_EP1_SYM: Path = P2K_DIR_EP1 / 'sym'
P2K_DIR_EP1_OBJ_OLD: Path = P2K_DIR_EP1_OBJ / 'old'
P2K_DIR_EP2: Path = P2K_TOOLCHAIN_ROOT / 'ep2'
P2K_DIR_EP2_DEF: Path = P2K_DIR_EP2 / 'def'
P2K_DIR_EP2_SRC: Path = P2K_DIR_EP2 / 'src'
P2K_DIR_EP2_TPL: Path = P2K_DIR_EP2 / 'tpl'
P2K_DIR_EP2_FPA: Path = P2K_DIR_EP2 / 'fpa'

P2K_TOOL_PAT: Path = P2K_DIR_TOOL_BIN / e_app('pat')
P2K_TOOL_POSTLINK: Path = P2K_DIR_TOOL_BIN / e_app('postlink')

P2K_EP1_ADS_TCC: Path = P2K_DIR_EP1_ADS_BIN / e_app('tcc')
P2K_EP1_ADS_TCPP: Path = P2K_DIR_EP1_ADS_BIN / e_app('tcpp')
P2K_EP1_ADS_ARMCC: Path = P2K_DIR_EP1_ADS_BIN / e_app('armcc')
P2K_EP1_ADS_ARMCPP: Path = P2K_DIR_EP1_ADS_BIN / e_app('armcpp')
P2K_EP1_ADS_ARMAR: Path = P2K_DIR_EP1_ADS_BIN / e_app('armar')
P2K_EP1_ADS_ARMASM: Path = P2K_DIR_EP1_ADS_BIN / e_app('armasm')
P2K_EP1_ADS_ARMLINK: Path = P2K_DIR_EP1_ADS_BIN / e_app('armlink')
P2K_EP1_ADS_FROMELF: Path = P2K_DIR_EP1_ADS_BIN / e_app('fromelf')

P2K_SDK_CONSTS_H: Path = P2K_DIR_EP_SDK / 'consts.h'
P2K_EP1_API_DEF: Path = P2K_DIR_EP1_DEF / 'ElfLoaderAPI1.def'
P2K_EP2_API_DEF: Path = P2K_DIR_EP2_DEF / 'ElfLoaderAPI2.def'
P2K_EP2_NMS_DEF: Path = P2K_DIR_EP2_DEF / 'EntriesNames.def'

ADS_SYM_FILE_HEADER: str = '#<SYMDEFS>#symdef-file'

MAX_BINARY_CHUNK_READ: int = 4096

P2K_ARGONLV_PHONES: list[str] = ['V3xx', 'V6', 'K3', 'Z9', 'V9']
