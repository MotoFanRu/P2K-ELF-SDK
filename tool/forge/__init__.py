# forge/__init__.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

from .arg import at_fw
from .arg import at_dir
from .arg import at_file
from .arg import at_hex

from .const import *

from .compiler import generate_source_with_const_chars
from .compiler import compile_c_ep1_ads_tcc
from .compiler import link_o_ep1_ads_armlink
from .compiler import bin_elf_ep1_ads_fromelf
from .compiler import assembly_asm_ep1_ads_armasm
from .compiler import packing_static_lib_ep1_ads_armar

from .file import delete_all_files_in_directory

from .firmware import parse_phone_firmware
from .firmware import parse_minor_major_firmware
from .firmware import get_file_size
from .firmware import determine_soc

from .hexer import hex2int
from .hexer import int2hex
from .hexer import int2hex_r
from .hexer import arrange16

from .libgen import libgen_ep1_fill_library_model
from .libgen import libgen_ep1_create_assembler_source
from .libgen import libgen_ep1_create_library

from .patcher import generate_fpa
from .patcher import bin2fpa

from .pattern import find_functions_from_patterns
from .pattern import append_pattern_to_file

from .sym import create_combined_sym_file
from .sym import split_and_validate_line
from .sym import validate_sym_file
from .sym import get_function_address_from_sym_file

from .util import format_timedelta
