# forge/constants.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import sys

from pathlib import Path


def is_win() -> bool:
	return sys.platform.startswith('win')


def e_app(executable_name: str) -> str:
	return (executable_name + '.exe') if is_win() else executable_name


P2K_TOOLCHAIN_ROOT: Path = Path('..')

P2K_DIR_EP_SDK: Path = P2K_TOOLCHAIN_ROOT / 'ep' / 'sdk'
P2K_DIR_TOOL: Path = P2K_TOOLCHAIN_ROOT / 'tool'
P2K_DIR_COMPILER: Path = P2K_DIR_TOOL / 'compiler'
P2K_DIR_EP1_ADS: Path = P2K_DIR_COMPILER / ('ep1_win_ADS' if is_win() else 'ep1_lin_ADS')
P2K_DIR_EP1_ADS_BIN: Path = P2K_DIR_EP1_ADS / 'bin'
P2K_DIR_EP1_FUNC: Path = P2K_DIR_TOOL / 'ep1_func'
P2K_DIR_EP1_OBJS: Path = P2K_DIR_TOOL / 'ep1_objs'
P2K_DIR_EP1_OBJS_OLD: Path = P2K_DIR_TOOL / 'ep1_objs' / 'old'

P2K_TOOL_PAT: Path = P2K_DIR_TOOL / e_app('pat')
P2K_EP1_ADS_TCC: Path = P2K_DIR_EP1_ADS_BIN / e_app('tcc')
P2K_EP1_ADS_TCPP: Path = P2K_DIR_EP1_ADS_BIN / e_app('tcpp')
P2K_EP1_ADS_ARMCC: Path = P2K_DIR_EP1_ADS_BIN / e_app('armcc')
P2K_EP1_ADS_ARMCPP: Path = P2K_DIR_EP1_ADS_BIN / e_app('armcpp')
P2K_EP1_ADS_ARMAR: Path = P2K_DIR_EP1_ADS_BIN / e_app('armar')
P2K_EP1_ADS_ARMASM: Path = P2K_DIR_EP1_ADS_BIN / e_app('armasm')
P2K_EP1_ADS_ARMLINK: Path = P2K_DIR_EP1_ADS_BIN / e_app('armlink')
P2K_EP1_ADS_FROMELF: Path = P2K_DIR_EP1_ADS_BIN / e_app('fromelf')

ADS_SYM_FILE_HEADER: str = '#<SYMDEFS>#symdef-file'
