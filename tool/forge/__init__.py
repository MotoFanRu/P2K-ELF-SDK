# forge/__init__.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

from .arguments import at_pfw
from .arguments import at_ffw
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

from .filesystem import delete_all_files_in_directory
from .filesystem import check_files_if_exists
from .filesystem import check_files_extensions
from .filesystem import check_directories_if_exists
from .filesystem import create_temporary_file_with_extension
from .filesystem import get_temporary_directory_path
from .filesystem import normalize_extension
from .filesystem import delete_file
from .filesystem import compare_paths

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
from .hexer import hex2hex

from .libgen import LibrarySort
from .libgen import libgen_version
from .libgen import ep1_libgen_asm
from .libgen import ep1_libgen_model
from .libgen import ep1_libgen_library
from .libgen import ep1_libgen_symbols
from .libgen import ep2_libgen_model
from .libgen import ep2_libgen_library
from .libgen import ep2_libgen_symbols
from .libgen import ep2_libgen_generate_names_defines
from .libgen import ep2_libgen_regenerator

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
from .symbols import dump_library_model_to_sym_file

from .types import LibraryModel

from .utilities import format_timedelta
from .utilities import chop_str
from .utilities import log_result
from .utilities import dump_text_file_to_debug_log
from .utilities import set_logging_configuration
from .utilities import get_current_datetime_formatted
