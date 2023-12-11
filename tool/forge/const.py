# forge/const.py

import sys
from pathlib import Path


def is_win() -> bool:
	return sys.platform.startswith('win')


P2K_TOOLCHAIN_ROOT = Path('..')

P2K_DIR_EP_SDK = P2K_TOOLCHAIN_ROOT / 'ep' / 'sdk'
P2K_DIR_TOOL = P2K_TOOLCHAIN_ROOT / 'tool'
P2K_DIR_COMPILER = P2K_DIR_TOOL / 'compiler'
P2K_DIR_EP1_ADS = P2K_DIR_COMPILER / ('ep1_win_ADS' if is_win() else 'ep1_lin_ADS')
P2K_DIR_EP1_FUNC = P2K_DIR_TOOL / 'ep1_func'
P2K_DIR_EP1_OBJS = P2K_DIR_TOOL / 'ep1_objs'
# P2K_DIR_EP1_OBJS = P2K_DIR_TOOL / 'ep1_objs' / 'old'

P2K_TOOL_PAT = P2K_DIR_TOOL / ('pat.exe' if is_win() else 'pat')
P2K_EP1_ADS_TCC = P2K_DIR_EP1_ADS / 'bin' / ('tcc.exe' if is_win() else 'tcc')
P2K_EP1_ADS_TCPP = P2K_DIR_EP1_ADS / 'bin' / ('tcpp.exe' if is_win() else 'tcpp')
P2K_EP1_ADS_ARMCC = P2K_DIR_EP1_ADS / 'bin' / ('armcc.exe' if is_win() else 'armcc')
P2K_EP1_ADS_ARMCPP = P2K_DIR_EP1_ADS / 'bin' / ('armcpp.exe' if is_win() else 'armcpp')
P2K_EP1_ADS_ARMAR = P2K_DIR_EP1_ADS / 'bin' / ('armar.exe' if is_win() else 'armar')
P2K_EP1_ADS_ARMASM = P2K_DIR_EP1_ADS / 'bin' / ('armasm.exe' if is_win() else 'armasm')
P2K_EP1_ADS_ARMLINK = P2K_DIR_EP1_ADS / 'bin' / ('armlink.exe' if is_win() else 'armlink')
P2K_EP1_ADS_FROMELF = P2K_DIR_EP1_ADS / 'bin' / ('fromelf.exe' if is_win() else 'fromelf')

ADS_SYM_FILE_HEADER = '#<SYMDEFS>#symdef-file'
