# forge/const.py

import sys
from pathlib import Path

P2K_TOOLCHAIN_ROOT = Path('..')

P2K_DIR_TOOL = P2K_TOOLCHAIN_ROOT / 'tool'
P2K_DIR_EP1_FUNC = P2K_DIR_TOOL / 'ep1_func'

P2K_TOOL_PAT_LIN = P2K_TOOLCHAIN_ROOT / 'tool' / 'pat'
P2K_TOOL_PAT_WIN = P2K_TOOLCHAIN_ROOT / 'tool' / 'pat.exe'
P2K_TOOL_PAT = P2K_TOOL_PAT_WIN if sys.platform.startswith('win') else P2K_TOOL_PAT_LIN

ADS_SYM_FILE_HEADER = '#<SYMDEFS>#symdef-file'
