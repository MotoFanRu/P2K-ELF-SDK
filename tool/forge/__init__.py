# forge/__init__.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

from .arguments import at_fw
from .arguments import at_dir
from .arguments import at_file
from .arguments import at_hex
from .arguments import at_fpa
from .arguments import at_path
from .arguments import at_hds

from .constants import *

from .toolchain import gen_src_const_chars
from .toolchain import ep1_ads_tcc
from .toolchain import ep1_ads_armlink
from .toolchain import ep1_ads_fromelf
from .toolchain import ep1_ads_armasm
from .toolchain import ep1_ads_armar

from .files import delete_all_files_in_directory
from .files import check_files_if_exists
from .files import check_files_extensions

from .firmware import parse_phone_firmware
from .firmware import parse_minor_major_firmware
from .firmware import get_file_size
from .firmware import determine_soc

from .hexer import hex2int
from .hexer import hex2int_r
from .hexer import int2hex
from .hexer import int2hex_r
from .hexer import arrange16
from .hexer import is_hex_string
from .hexer import normalize_hex_string
from .hexer import normalize_hex_address

from .libgen import ep1_libgen_asm
from .libgen import ep1_libgen_model
from .libgen import ep1_libgen_library

from .patcher import bin2fpa
from .patcher import hex2fpa
from .patcher import fpa2bin
from .patcher import unite_fpa_patches
from .patcher import apply_fpa_patch

from .patterns import pat_find
from .patterns import pat_append

from .symbols import create_combined_sym_file
from .symbols import split_and_validate_line
from .symbols import validate_sym_file
from .symbols import get_function_address_from_sym_file

from .utilities import format_timedelta
from .utilities import chop_str
