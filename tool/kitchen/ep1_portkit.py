#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A PortKit Utility for building ElfPack v1.x for Motorola phones on P2K platform.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

import sys
import logging
import argparse

import forge

from pathlib import Path
from datetime import datetime
from argparse import Namespace


# Constants.
FUNC_REGISTER: str = 'Register'
FUNC_AUTORUN: str = 'AutorunMain'

EP1_PFW_VARIANTS: dict[str, dict[str, any]] = {
	'R365_G_0B.D3.08R': {
		'opts_all':       ['-DFTR_C650'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.x patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'C650_R365_G_0B.D3.08R.pts',
		'firmware':       forge.P2K_DIR_CG / 'C650_R365_G_0B.D3.08R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  True,        # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'a'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R373_G_0E.30.49R': {
		'opts_all':       ['-DFTR_E1'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.x patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'E1_R373_G_0E.30.49R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R373_G_0E.30.79R': {
		'opts_all':       ['-DFTR_E1'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.x patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'E1_R373_G_0E.30.79R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R373_G_0E.30.DAR_test16': {
		'opts_all':       ['-DFTR_E1'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    0x003137B4,  # ElfPack v1.x patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'E1_R373_G_0E.30.DAR_test16.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R373_G_0E.30.DAR_test17': {
		'opts_all':       ['-DFTR_E1'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    0x003137B4,  # ElfPack v1.x patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'E1_R373_G_0E.30.DAR_test17.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452F_G_08.03.08R': {
		'opts_all':       ['-DFTR_K1'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x0151C350,  # ElfPack v1.x patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'K1_R452F_G_08.03.08R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R3511_G_0A.52.45R_A': {
		'opts_all':       ['-DFTR_L6'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.x patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'L6_R3511_G_0A.52.45R_A.pts',
		'firmware':       forge.P2K_DIR_CG / 'L6_R3511_G_0A.52.45R_A.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R3443H1_G_0A.65.0BR': {
		'opts_all':       ['-DFTR_L6I'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.x patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'L6i_R3443H1_G_0A.65.0BR.pts',
		'firmware':       forge.P2K_DIR_CG / 'L6i_R3443H1_G_0A.65.0BR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R4513_G_08.B7.ACR_RB': {
		'opts_all':       ['-DFTR_L7'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x00CA01B0,  # ElfPack v1.x patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'L7_R4513_G_08.B7.ACR_RB.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R4513_G_08.B7.E0R_RB': {
		'opts_all':       ['-DFTR_L7'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x00C90730,  # ElfPack v1.x patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'L7_R4513_G_08.B7.E0R_RB.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452D_G_08.01.0AR': {
		'opts_all':       ['-DFTR_L7E'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x0021D340,  # ElfPack v1.x patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'L7e_R452D_G_08.01.0AR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452J_G_08.22.05R': {
		'opts_all':       ['-DFTR_L9'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x0152E010,  # ElfPack v1.x patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'L9_R452J_G_08.22.05R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R4441D_G_08.01.03R': {
		'opts_all':       ['-DFTR_V3I'],
		'addr_start':     0x100A0000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.x patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'V3i_R4441D_G_08.01.03R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R4515_G_08.BD.D3R': {
		'opts_all':       ['-DFTR_V3R'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.x patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'V3r_R4515_G_08.BD.D3R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R3512_G_0A.30.6CR': {
		'opts_all':       ['-DFTR_V235'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.x patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'V235_R3512_G_0A.30.6CR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R4513_G_08.B7.ACR': {
		'opts_all':       ['-DFTR_V360'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.x patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'V360_R4513_G_08.B7.ACR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'TRIPLETS_G_0B.09.72R': {
		'opts_all':       ['-DFTR_V600'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    0x00C3C1B0,  # ElfPack v1.x patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'V600_TRIPLETS_G_0B.09.72R.pts',
		'firmware':       forge.P2K_DIR_CG / 'V600_TRIPLETS_G_0B.09.72R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'a'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452B_G_08.02.0DR': {
		'opts_all':       ['-DFTR_Z3'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x0021D290,  # ElfPack v1.x patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'Z3_R452B_G_08.02.0DR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452F1_G_08.04.09R': {
		'opts_all':       ['-DFTR_Z3'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.x patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'Z3_R452F1_G_08.04.09R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452H6_G_08.00.05R': {
		'opts_all':       ['-DFTR_Z3'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x0021D290,  # ElfPack v1.x patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts',
		'firmware':       forge.P2K_DIR_CG / 'Z3_R452H6_G_08.00.05R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R261171LD_U_99.51.06R': {
		'opts_all':       ['-DEA1', '-DUSE_UIS_ALLOCA', '-DFTR_K3'],
		'addr_start':     0xA0080000,  # Firmware start address.
		'addr_offset':    0x014B0B18,  # ElfPack v1.x patch address.
		'patterns':       None,
		'firmware':       forge.P2K_DIR_CG / 'K3_R261171LD_U_99.51.06R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'b'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R474_G_08.48.6FR': {
		'opts_all':       ['-DUSE_UIS_ALLOCA', '-DFTR_V635'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    0x009E9100,  # ElfPack v1.x patch address.
		'patterns':       None,
		'firmware':       forge.P2K_DIR_CG / 'V635_R474_G_08.48.6FR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  True,        # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'drive_patch':    'a'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
}


# Patches.
def apply_patches(phone: str, firmware: str, lib_sym: Path) -> bool:
	patches: list[str] = []
	if phone == 'E1':
		if firmware == 'R373_G_0E.30.49R':
			# Pattern: EV_BacklightContinueOn D 00000E10102E
			patches.append('0x102FC120 D EV_BacklightContinueOn')
		elif firmware == 'R373_G_0E.30.DAR':
			patches.append('0x03FEB4E4 T suAllocMem')
			patches.append('0x03FEB6EA T suFreeMem')
			patches.append('0x03FEC954 A __rt_memclr')
			patches.append('0x03FF0E40 A __rt_memset')
			patches.append('0x03FF0E40 A _rt_memset')
	elif phone == 'K1':
		if firmware == 'R452F_G_08.03.08R':
			# Pattern: [201490002000900190029003+0x01E8]+28
			patches.append('0x14501210 D Ram_l7e')
	elif phone == 'L9':
		if firmware == 'R452J_G_08.22.05R':
			# Pattern: [7FFF0000011E00000122+0x0A]
			patches.append('0x1451C1C8 D Ram_l7e')
	elif phone == 'V3i':
		if firmware == 'R4441D_G_08.01.03R':
			# Pattern: BEGIN_4A__IN_DB D 1 BC08471800000600+0x4
			patches.append('0x100A7AB6 D BEGIN_4A__IN_DB')
	elif phone == 'V235':
		if firmware == 'R3512_G_0A.30.6CR':
			# Pattern: Ram_398_l7 D [80A842B0D1062006+0x24]+0x10
			patches.append('0x124A4BB8 D Ram_398_l7')
	elif phone == 'Z3':
		if firmware == 'R452F1_G_08.04.09R':
			# Pattern: [14??????00003E580000FFFF]+0x4
			patches.append('0x14076374 D Ram_l7e')
	return forge.libgen_apply_patches(patches, lib_sym, phone, firmware, 'EP1') if patches else False


# Various generators.
def generate_lib_sym(
	p_i_f: Path, p_i_e: Path, p_o_l: Path,
	names_skip: list[str], patterns_add: list[str],
	names_skip_pattern: bool = False
) -> bool:
	if forge.check_files_if_exists([p_i_f, p_i_e]):
		with (p_i_f.open(mode='r') as i_f, p_i_e.open(mode='r') as i_s, p_o_l.open(mode='w', newline='\r\n') as o_l):
			o_l.write(f'{forge.ADS_SYM_FILE_HEADER}\n')
			o_l.write('# SYMDEFS ADS HEADER\n\n')
			for line in i_f.read().splitlines():
				address, mode, name = forge.split_and_validate_line(line)
				found: bool = False
				if names_skip_pattern:
					if name is not None:
						for add in names_skip:
							if name.find(add) != -1:
								found = True
				if (name is not None) and (not found) if names_skip_pattern else (name not in names_skip):
					o_l.write(f'{line}\n')
			o_l.write('\n\n')
			for line in i_s.read().splitlines():
				address, mode, name = forge.split_and_validate_line(line)
				if name is not None:
					for add in patterns_add:
						if name.find(add) != -1:
							o_l.write(f'{line}\n')
			return True
	return False


def generate_register_patch(fw: str, author: str, desc: str, p_e: Path, p_r: Path, p_p: Path, cg: Path) -> bool:
	if forge.check_files_if_exists([p_e, p_r]):
		hex_data: str = forge.int2hex_r(forge.get_function_address_from_sym_file(p_e, FUNC_AUTORUN))
		reg_address: int = forge.get_function_address_from_sym_file(p_r, FUNC_REGISTER)

		forge.hex2fpa(fw, author, desc, reg_address, hex_data, p_p, cg)
		return True
	return False


def generate_system_information_source(phone: str, firmware: str, soc: str, source_file: Path) -> bool:
	system_info: dict[str, str] = {}
	major, minor = forge.parse_minor_major_firmware(firmware)
	system_info['n_phone'] = phone
	system_info['n_platform'] = soc
	system_info['n_majorfw'] = major
	system_info['n_minorfw'] = minor
	return forge.gen_src_const_chars(source_file, system_info)


def generate_register_sym(
	combined_sym: Path, cgs_path: Path, register_func: str, pat: Path, sym: Path, hook: int = None
) -> bool:
	address: int = hook if hook else forge.get_function_address_from_sym_file(combined_sym, register_func)
	if address != 0x00000000:
		forge.pat_append(pat, 'Register', 'D', forge.int2hex_r(address))
		forge.pat_find(pat, cgs_path, 0x00000000, False, sym)
		if forge.ep1_libgen_model(sym, forge.LibrarySort.NAME) is not None:
			return True
	else:
		logging.error(f'Cannot find address of "{register_func}" function')
	return False


# PortKit ARM v1.x working flow.
def start_ep1_portkit_work(opts: dict[str, any]) -> bool:
	logging.info('Start building ElfPack v1.x for the Motorola P2K phone.')
	logging.info('')
	logging.info('Parameters:')
	forge.args_dump(opts)
	logging.info('')
	logging.info('Prepare PortKit environment.')
	forge.prepare_clean_output_directory(opts["output"], True)
	logging.info('')

	val_combined_sym: Path = opts['output'] / 'Combined.sym'

	if not opts['precached']:
		logging.info('Finding SoC related functions from patterns.')
		val_lte1_pat: Path = forge.P2K_DIR_EP1_PAT / 'General_P2K_LTE.pts'
		val_lte2_pat: Path = forge.P2K_DIR_EP1_PAT / 'General_P2K_LTE2.pts'
		val_lte2_modern_pat: Path = forge.P2K_DIR_EP1_PAT / 'General_P2K_LTE2_Modern.pts'
		val_lte2_irom_sym: Path = forge.P2K_DIR_EP1_SYM / 'General_P2K_LTE2_IROM.sym'
		val_platform_sym: Path = opts['output'] / 'Platform.sym'
		val_functions_sym: Path = opts['output'] / 'Functions.sym'
		val_functions_modern_lte2: Path = opts['output'] / 'Functions_P2K_LTE2_Modern.sym'
		if (opts['soc'] == 'LTE') and not opts['skip_platform']:
			forge.pat_find(val_lte1_pat, opts['fw_file'], opts['start'], False, val_platform_sym)
		elif (opts['soc'] == 'LTE2') and not opts['skip_platform']:
			forge.pat_find(val_lte2_pat, opts['fw_file'], opts['start'], False, val_platform_sym)
			if forge.is_modern_lte2(opts['phone']):
				forge.pat_find(val_lte2_modern_pat, opts['fw_file'], opts['start'], False, val_functions_modern_lte2)
		else:
			val_functions_sym = val_combined_sym
			logging.warning('Will skip generating platform symbols file.')
		logging.info('')

		logging.info('Finding general functions from patterns.')
		forge.pat_find(opts['patterns'], opts['fw_file'], opts['start'], opts['ram_trans'], val_functions_sym)
		logging.info('')

		if not opts['skip_platform']:
			logging.info('Combining all functions into one symbols file.')
			if opts['soc'] == 'LTE':
				forge.create_combined_sym_file([val_functions_sym, val_platform_sym], val_combined_sym)
			elif opts['soc'] == 'LTE2':
				if forge.is_modern_lte2(opts['phone']):
					forge.create_combined_sym_file(
						[val_functions_sym, val_platform_sym, val_functions_modern_lte2, val_lte2_irom_sym],
						val_combined_sym
					)
				else:
					forge.create_combined_sym_file(
						[val_functions_sym, val_platform_sym, val_lte2_irom_sym],
						val_combined_sym
					)
			logging.info('')

		if opts['append']:
			logging.info('Append additional patterns file if any.')
			val_append_sym: Path = opts['output'] / 'Append.sym'
			val_combined_append_sym: Path = opts['output'] / 'Combined_Append.sym'
			forge.pat_find(opts['append'], opts['fw_file'], opts['start'], opts['ram_trans'], val_append_sym)
			forge.create_combined_sym_file([val_append_sym, val_combined_sym], val_combined_append_sym)
			val_combined_sym = val_combined_append_sym
			logging.info('')

		logging.info('Applying phone specific patches.')
		apply_patches(opts['phone'], opts['fw_name'], val_combined_sym)
		logging.info('')

		logging.info('Validating combined symbols file.')
		if not forge.validate_sym_file(val_combined_sym):
			return False
		else:
			logging.info(f'The "{val_combined_sym}" sym file is validated.')
		logging.info('')
	else:
		if not forge.check_files_if_exists([opts['precached']]):
			logging.error(f'cannot find {opts["precached"]} file with entity addresses')
			return False
		selection: list[str] = forge.ep1_libgen_names_sym(forge.P2K_DIR_EP1_DEF / 'NeededFunctions.def')
		if opts['use_afw_wraps']:
			selection.append('AFW_CreateInternalQueuedEvPriv')
		else:
			selection.append('AFW_CreateInternalQueuedEvAux')
			selection.append('AFW_CreateInternalQueuedEvAuxD')
		if opts['new_obj']:
			selection.append('DL_FsFFileExist')
			selection.append('u_strcpy')
		if '-DLOG_TO_FILE' in opts['opts_all']:
			selection.append('DL_FsWriteFile')
		if '-DUSE_UIS_ALLOCA' in opts['opts_all']:
			selection.append('uisAllocateMemory')
			selection.append('uisFreeMemory')
		forge.ep1_libgen_chunk_sym(opts['precached'], val_combined_sym, forge.LibrarySort.NAME, selection, opts['pfw'])

	if opts['append_sym']:
		logging.info('Append additional symbols file if any.')
		val_combined_append_added_sym: Path = opts['output'] / 'Combined_Append_Added.sym'
		forge.create_combined_sym_file([opts['append_sym'], val_combined_sym], val_combined_append_added_sym)
		val_combined_sym = val_combined_append_added_sym
		logging.info('')

	if opts['gcc']:
		functions, library_model = forge.ep1_libgen_model(val_combined_sym, forge.LibrarySort.NAME)
		forge.ep1_libgen_asm(opts['output'] / 'LibStubGCC.S', library_model, False, True, False)

	logging.info('Generating register symbols file.')
	val_register_sym: Path = opts['output'] / 'Register.sym'
	val_register_pat: Path = opts['output'] / 'Register.pts'
	val_found: bool = generate_register_sym(
		val_combined_sym, opts['fw_file'], opts['inject'], val_register_pat, val_register_sym, opts['hook']
	)
	if not val_found:
		logging.error(f'Cannot generate "{val_register_pat}" and "{val_register_sym}" files.')
		return False
	logging.info('')

	logging.info('Generating system information C-source file.')
	val_system_info_c: Path = opts['output'] / 'SysInfo.c'
	val_system_info_o: Path = opts['output'] / 'SysInfo.o'
	generate_system_information_source(opts['phone'], opts['fw_name'], opts['soc'], val_system_info_c)
	logging.info('')

	val_src_dir: Path = (forge.P2K_DIR_EP1_SRC / 'goldsrc') if opts['goldsrc'] else forge.P2K_DIR_EP1_SRC
	c_flags: list[str] =  ['-DEG1'] if opts['gcc'] else ['-DEP1']
	c_flags.extend(opts['opts_all'])
	gcc: bool = opts['gcc']
	if gcc:
		logging.info('Compiling S-source and C-source files using GCC compiler.')
	else:
		logging.info('Compiling C-source files using ADS compiler.')
	forge.toolchain_compile(val_system_info_c, val_system_info_o, True, c_flags, opts['gcc'], opts['argon'])
	if opts['compile']:
		if opts['gcc']:
			forge.toolchain_compile(
				opts['output'] / 'LibStubGCC.S', opts['output'] / 'LibStubGCC.o',
				True, c_flags, opts['gcc'], opts['argon']
			)
		forge.toolchain_compile(
			val_src_dir / 'AutoRun.c', opts['output'] / 'AutoRun.o',
			True, c_flags, opts['gcc'], opts['argon']
		)
		forge.toolchain_compile(
			val_src_dir / 'ElfLoader.c', opts['output'] / 'ElfLoader.o',
			True, c_flags, opts['gcc'], opts['argon']
		)
		forge.toolchain_compile(
			val_src_dir / 'ElfLoaderApp.c', opts['output'] / 'ElfLoaderApp.o',
			True, c_flags, opts['gcc'], opts['argon']
		)
		forge.toolchain_compile(
			val_src_dir / 'AFW_CreateInternalQueuedEv_Wrappers.c',
			opts['output'] / 'AFW_CreateInternalQueuedEv_Wrappers.o',
			True, c_flags, opts['gcc'], opts['argon']
		)
	logging.info('')

	logging.info('Linking object files to binary.')
	val_object_path: Path = (forge.P2K_DIR_EP1_OBJ / 'new') if opts['new_obj'] else forge.P2K_DIR_EP1_OBJ
	if opts['compile']:
		val_object_path = opts['output']
	# WARNING: Order is important here!
	val_link_objects: list[Path] = [
		val_object_path / 'AutoRun.o',
		val_object_path / 'ElfLoaderApp.o',
		val_object_path / 'ElfLoader.o',
		val_system_info_o,
	]
	if not opts['gcc']:
		val_link_objects.append(forge.P2K_DIR_EP1_LIB / 'libarm_small.a')
		val_link_objects.append(val_combined_sym)
	else:
		val_link_objects.append(val_object_path / 'LibStubGCC.o')
	if opts['use_afw_wraps']:
		val_link_objects.insert(3, Path(val_object_path / 'AFW_CreateInternalQueuedEv_Wrappers.o'))
	val_elfpack_elf: Path = opts['output'] / 'ElfPack.elf'
	val_elfpack_sym: Path = opts['output'] / 'ElfPack.sym'
	val_elfpack_bin: Path = opts['output'] / 'ElfPack.bin'
	if not opts['gcc']:
		val_link_res = forge.ep1_ads_armlink(
			val_link_objects, val_elfpack_elf, opts['address'], val_elfpack_sym, ['-entry', FUNC_AUTORUN ]
		)
		if not val_link_res:
			logging.error(f'Cannot link "{val_elfpack_elf}" executable file using ADS.')
			return False
		forge.ep1_ads_fromelf(val_elfpack_elf, val_elfpack_bin)
	else:
		val_ld_script_tpl = forge.P2K_DIR_EP1_TPL / ('ElfPackArgon.tpl' if opts['argon'] else 'ElfPackNeptune.tpl')
		val_ld_script_org = opts['output'] / 'ElfPack.ld'
		forge.patch_text_file_template(
			val_ld_script_tpl, val_ld_script_org,
			{'%addr_entry%' : forge.int2hex(opts['address']) }
		)
		if not forge.ep2_gcc_link(val_link_objects, val_elfpack_elf, True, val_ld_script_org, None, opts['argon']):
			logging.error(f'Cannot link "{val_elfpack_elf}" executable file using GCC.')
			return False
		forge.ep2_gcc_objcopy(val_elfpack_elf, val_elfpack_bin)
		forge.ep2_gcc_nm(val_elfpack_elf, opts['output'] / 'ElfPack.nm')
		forge.convert_nm_to_sym(opts['output'] / 'ElfPack.nm', opts['output'] / 'ElfPack.sym')
	logging.info('')

	logging.info('Patch resulting binaries.')
	d: str = forge.str2hex(opts["drive"])
	# "file://b/Elf/elfloader.lib" or "/b/Elf/elfloader.lib" for the new P2K phones.
	# Pattern: 66696C653A2F2F622F456C662F656C666C6F616465722E6C6962
	po1: str = '66696C653A2F2F622F456C662F656C666C6F616465722E6C6962'
	pn1: str = f'66696C653A2F2F{d}2F456C662F656C666C6F616465722E6C6962'
	pa1: str = f'2F{d}2F456C662F656C666C6F616465722E6C6962000000000000'
	forge.patch_binary_file_res(val_elfpack_bin, po1, pa1 if opts['argon'] else pn1)
	# "f.i.l.e.:././.b./.E.l.f./.a.u.t.o...r.u.n." or "/.b./.E.l.f./.a.u.t.o...r.u.n." for the new P2K phones.
	# Pattern: 660069006C0065003A002F002F0062002F0045006C0066002F006100750074006F002E00720075006E00
	po2: str = '660069006C0065003A002F002F0062002F0045006C0066002F006100750074006F002E00720075006E00'
	pn2: str = f'660069006C0065003A002F002F00{d}002F0045006C0066002F006100750074006F002E00720075006E00'
	pa2: str = f'2F00{d}002F0045006C0066002F006100750074006F002E00720075006E00000000000000000000000000'
	forge.patch_binary_file_res(val_elfpack_bin, po2, pa2 if opts['argon'] else pn2)
	logging.info('')

	logging.info('Creating Flash&Backup 3 patches.')
	val_register_fpa: Path = opts['output'] / 'Register.fpa'
	val_elfpack_fpa: Path = opts['output'] / 'ElfPack.fpa'
	val_elfdir_fpa: Path = opts['output'] / 'ElfDirectory.fpa'
	val_result_fpa: Path = opts['output'] / 'Result.fpa'
	forge.bin2fpa(
		opts['fw_name'], 'Andy51', 'ElfPack v1.x', opts['offset'], val_elfpack_bin, val_elfpack_fpa, opts['fw_file']
	)
	generate_register_patch(
		opts['fw_name'], 'Andy51', 'ElfPack v1.x Register patch',
		val_elfpack_sym, val_register_sym, val_register_fpa, opts['fw_file']
	)
	patches: list[Path] = [val_register_fpa, val_elfpack_fpa]
	if opts['directory']:
		desc: str = 'ElfPack v1.x Directory patch'
		logging.info(f'Generate {desc} for "mixedmedia" => "Elf" directory: "{val_elfdir_fpa}".')
		po3: str = (
			'006D0069007800650064006D00650064'
			'00690061000000000000000000000000'
			'00000000000000000000000000000000'
			'00000000000000000000000000000000'
			'0000000000000000000000160002'
		)
		pn3: str = (
			'0045006C006600000000000000000000'
			'00000000000000000000000000000000'
			'00000000000000000000000000000000'
			'00000000000000000000000000000000'
			'0000000000000000000000100001'
		)
		offset: int = forge.patch_binary_file(opts['fw_file'], po3, pn3, True)
		if offset > 0:
			if forge.hex2fpa(opts['fw_name'], 'EXL', desc, offset, pn3, val_elfdir_fpa, opts['fw_file']):
				patches.append(val_elfdir_fpa)
		else:
			logging.info(f'Cannot find original patch data in "{opts["fw_file"]}" file.')
			logging.info(f'Data: {po3}')
			return False
	forge.unite_fpa_patches(opts['fw_name'], 'Andy51, EXL', 'Combined ElfPack v1.x patch.', patches, val_result_fpa)
	logging.info('')

	logging.info('Creating ElfPack v1.x library for Phone.')
	val_library_sym: Path = opts['output'] / 'Library.sym'
	val_library_asm: Path = opts['output'] / ('LibStaticGCC.S' if opts['gcc'] else 'LibStaticADS.asm')
	val_so_library_asm: Path = opts['output'] / 'LibSharedGCC.S'
	val_elfloader_lib: Path = opts['output'] / 'elfloader.lib'
	if not opts['gcc']:
		generate_lib_sym(
			forge.ep1_libgen_get_library_sym(opts['pfw']) if opts['precached'] else val_combined_sym,
			val_elfpack_sym, val_library_sym,
			['Ldr', 'UtilLogStringData', 'namecmp', 'u_utoa', '_ll_cmpu'] if opts['precached'] else
				[opts['inject'], 'APP_CALC_MainRegister', '_region_table'],
			['Ldr', 'UtilLogStringData', 'namecmp', 'u_utoa', '_ll_cmpu'],
			opts['precached']
		)
	else:
		generate_lib_sym(
			forge.ep1_libgen_get_library_sym(opts['pfw']), val_elfpack_sym, val_library_sym,
			['Ldr', 'UtilLogStringData', 'namecmp', 'u_utoa', '_ll_cmpu'],
			['Ldr', 'UtilLogStringData', 'namecmp', 'u_utoa', '_ll_cmpu'],
			True
		)
	functions, library_model = forge.ep1_libgen_model(val_library_sym, forge.LibrarySort.NAME)
	forge.ep1_libgen_asm(val_library_asm, library_model, opts['gcc'])
	forge.ep1_libgen_library(val_elfloader_lib, library_model, functions, opts['argon'])
	forge.ep1_libgen_symbols(val_elfloader_lib, val_library_sym, forge.LibrarySort.NAME, opts['phone'], opts['fw_name'])
	logging.info('')

	logging.info('Compiling ElfPack v1.x library for SDK.')
	val_a_library_obj: Path = opts['output'] / ('libeg1_gcc.o' if opts['gcc'] else 'libep1_ads.o')
	val_so_library_obj: Path = opts['output'] / 'libeg1_gcc_stub.o'
	val_libep1_static_lib: Path = opts['output'] / ('libeg1_gcc.a' if opts['gcc'] else 'libep1_ads.a')
	val_libep1_shared_lib: Path = opts['output'] / 'libeg1_gcc_stub.so'
	val_important_dev_lib: Path = val_libep1_static_lib if not opts['gcc'] else val_libep1_shared_lib
	if not opts['gcc']:
		forge.ep1_ads_armasm(val_library_asm, val_a_library_obj)
		forge.ep1_ads_armar([val_a_library_obj], val_libep1_static_lib)
	else:
		forge.toolchain_compile(val_library_asm, val_a_library_obj, True, c_flags, opts['gcc'], opts['argon'])
		forge.ep2_gcc_ar([val_a_library_obj], val_libep1_static_lib)
		logging.info(f'Creating "{val_libep1_shared_lib}" shared stub library...')
		forge.ep1_libgen_asm(val_so_library_asm, library_model, True, False, True, True)
		c_flags.extend(['-fPIC'])
		forge.toolchain_compile(val_so_library_asm, val_so_library_obj, True, c_flags, opts['gcc'], opts['argon'])
		c_flags.extend(['-shared', '-fPIC', f'-Wl,--soname,{val_libep1_shared_lib.name}'])
		forge.ep2_gcc_link([val_so_library_obj], val_libep1_shared_lib, True, None, c_flags, opts['argon'])
		forge.ep2_gcc_strip(val_libep1_shared_lib)
	logging.info('')

	logging.info('ElfPack v1.x building report:')
	logging.info('')
	logging.info('Important files:')
	logging.info(f'\t{str(val_elfloader_lib):<40} - Library for "{opts["phone"]}" on "{opts["fw_name"]}" firmware.')
	logging.info(f'\t{str(val_result_fpa):<40} - Generated ElfPack v1.x combined patch for Flash&Backup.')
	logging.info('')
	logging.info('Developer files:')
	logging.info(f'\t{str(val_important_dev_lib):<40} - Compiled library for SDK.')
	logging.info(f'\t{str(val_library_sym):<40} - Generated library entities list.')
	logging.info('')

	return True


# Arguments parsing routines.
class Args(argparse.ArgumentParser):
	def error(self, message: str) -> None:
		self.print_help(sys.stderr)
		self.exit(2, f'{self.prog}: error: {message}\n')

	def parse_check_arguments(self) -> dict[str, any]:
		opts: dict[str, any] = {}
		args: Namespace = self.parse_args()

		phone, firmware = args.phone_fw
		variants: dict[str, any] = EP1_PFW_VARIANTS.get(firmware, None)
		if not variants:
			self.error(f'unknown {phone} phone and {firmware} firmware')

		opts['verbose'] = args.verbose
		opts['search'] = args.search
		opts['skip_platform'] = args.skip_platform
		opts['compile'] = not args.objects
		opts['gcc'] = args.gcc
		if opts['gcc'] and not opts['compile']:
			self.error('cannot use "-g" flag (Compile using GCC) with "-j" flag (Use Precompiled Object Files)')
		opts['goldsrc'] = args.goldsrc
		if opts['goldsrc'] and not opts['compile']:
			self.error('cannot use "-d" flag (Compile Original Sources) with "-j" flag (Use Precompiled Object Files)')
		opts['new_obj'] = args.new_objects
		if opts['new_obj'] and opts['compile']:
			self.error('cannot use "-n" flag (Use New Object Files) without "-j" flag (Use Precompiled Object Files)')
		opts['ram_trans'] = not args.no_ram_trans
		if not opts['ram_trans'] and not opts['search']:
			self.error('cannot use "-m" flag (do not RAM Trans) without "-B" flag (Binary Pattern Search)')
		opts['output'] = args.output if args.output else Path('build')
		opts['directory'] = not args.skip_patch_dir
		if args.append_pts and not args.patterns:
			self.error('cannot use "-a" flag (Append Patterns File) without "-p" flag (Override Patterns Path)')
		opts['patterns'] = args.patterns if args.patterns else variants['patterns']
		if args.append_pts:
			opts['patterns'] = \
				variants['patterns'] if variants['patterns'] else forge.P2K_DIR_EP1_PAT / 'General_P2K_Neptune.pts'
			opts['append'] = args.patterns
		else:
			opts['append'] = None
		opts['append_sym'] = args.append_sym
		opts['fw_file'] = args.firmware if args.firmware else variants['firmware']

		opts['start'] = args.start if args.start else variants['addr_start']
		opts['offset'] = args.offset if args.offset else variants['addr_offset']
		if not opts['offset']:
			opts['offset'] = forge.arrange16(forge.get_file_size(opts['fw_file']))
		opts['hook'] = args.hook
		opts['address'] = opts['start'] + opts['offset']
		opts['soc'] = forge.determine_soc(opts['start'])
		opts['phone'] = phone
		opts['fw_name'] = firmware
		opts['pfw'] = args.phone_fw
		opts['argon'] = phone in forge.P2K_ARGONLV_PHONES

		opts['inject'] = variants['func_inject']

		if opts['skip_platform'] and not opts['search']:
			self.error('cannot use "-k" flag (Skip Platform Searching) without "-B" flag (Binary Pattern Search)')
		if not opts['patterns'] and opts['search']:
			self.error('patterns file is not provided, use "-p FILE.pts" option')
		opts['precached'] = \
			forge.ep1_libgen_get_library_sym(opts['pfw']) if (not opts['patterns'] or not opts['search']) else None
		opts['use_afw_wraps'] = variants['use_afw_wraps']
		opts['opts_all'] = variants['opts_all']
		opts['drive'] = variants['drive_patch']

		return opts


def parse_arguments() -> dict[str, any]:
	hlp: dict[str, str] = {
		'D': 'A PortKit Utility for building ElfPack v1.x for Motorola phones on P2K platform, 01-Jan-2025',
		'P': 'set phone and firmware, e.g., "E1_R373_G_0E.30.49R"',
		'g': 'use ARM GCC for compilation (default is ARM ADS)',
		'd': 'use original source code without modifications',
		'B': 'search for binary function patterns in firmware file',
		'm': 'do not resolve precached IRAM function addresses while searching',
		'k': 'skip platform binary patterns searching',
		't': 'skip "mixedmedia" => "Elf" directory patching',
		'j': 'use precompiled object files instead of building sources',
		'n': 'use new precompiled object files (use with -j)',
		'f': 'override path to CG0+CG1 firmware file',
		'p': 'override path to patterns file',
		'a': 'append patterns file instead of overriding it (use with -p)',
		'y': 'append additional symbols file',
		's': 'override start address of CG0+CG1 firmware (in HEX)',
		'x': 'override result patch offset in CG0+CG1 file (in HEX)',
		'u': 'override inject hook register function address (in HEX)',
		'o': 'set output artifacts directory',
		'v': 'enable verbose output'
	}
	epl: str = """examples:
	# Build ElfPack v1.x and libraries for target:
	python ep1_portkit.py -P E1_R373_G_0E.30.49R
	python ep1_portkit.py -P E1_R373_G_0E.30.79R
	python ep1_portkit.py -P E1_R373_G_0E.30.DAR_test16 -B -t -a -p ../../ep1/pts/E1_R373_G_0E.30.DAR.pts
	python ep1_portkit.py -P E1_R373_G_0E.30.DAR_test17 -B -t -a -p ../../ep1/pts/E1_R373_G_0E.30.DAR.pts
	python ep1_portkit.py -P K1_R452F_G_08.03.08R
	python ep1_portkit.py -P L6_R3511_G_0A.52.45R_A
	python ep1_portkit.py -P L6i_R3443H1_G_0A.65.0BR
	python ep1_portkit.py -P L7_R4513_G_08.B7.ACR_RB
	python ep1_portkit.py -P L7_R4513_G_08.B7.E0R_RB
	python ep1_portkit.py -P L7e_R452D_G_08.01.0AR
	python ep1_portkit.py -P L9_R452J_G_08.22.05R
	python ep1_portkit.py -P V3i_R4441D_G_08.01.03R
	python ep1_portkit.py -P V3r_R4515_G_08.BD.D3R
	python ep1_portkit.py -P V235_R3512_G_0A.30.6CR
	python ep1_portkit.py -P V360_R4513_G_08.B7.ACR
	python ep1_portkit.py -P V600_TRIPLETS_G_0B.09.72R
	python ep1_portkit.py -P Z3_R452B_G_08.02.0DR
	python ep1_portkit.py -P Z3_R452F1_G_08.04.09R
	python ep1_portkit.py -P Z3_R452H6_G_08.00.05R
	python ep1_portkit.py -P C650_R365_G_0B.D3.08R
	python ep1_portkit.py -P K3_R261171LD_U_99.51.06R -g
	python ep1_portkit.py -P V635_R474_G_08.48.6FR -g

	# Find functions and build ElfPack v1.x and libraries for target:
	python ep1_portkit.py -P E1_R373_G_0E.30.49R -B
	python ep1_portkit.py -P E1_R373_G_0E.30.49R -B -m
	python ep1_portkit.py -P E1_R373_G_0E.30.49R -B -p FILE.pts
	python ep1_portkit.py -P E1_R373_G_0E.30.49R -B -a -p FILE.pts
	python ep1_portkit.py -P E1_R373_G_0E.30.49R -B -a -p FILE.pts -y FILE.sym
	"""
	p_args: Args = Args(description=hlp['D'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	p_args.add_argument('-P', '--phone-fw', required=True, type=forge.at_pfw, metavar='PHONE_FW', help=hlp['P'])
	p_args.add_argument('-g', '--gcc', required=False, action='store_true', help=hlp['g'])
	p_args.add_argument('-d', '--goldsrc', required=False, action='store_true', help=hlp['d'])
	p_args.add_argument('-B', '--search', required=False, action='store_true', help=hlp['B'])
	p_args.add_argument('-m', '--no-ram-trans', required=False, action='store_true', help=hlp['m'])
	p_args.add_argument('-k', '--skip-platform', required=False, action='store_true', help=hlp['k'])
	p_args.add_argument('-t', '--skip-patch-dir', required=False, action='store_true', help=hlp['t'])
	p_args.add_argument('-j', '--objects', required=False, action='store_true', help=hlp['j'])
	p_args.add_argument('-n', '--new-objects', required=False, action='store_true', help=hlp['n'])
	p_args.add_argument('-f', '--firmware', required=False, type=forge.at_ffw, metavar='FILE.smg', help=hlp['f'])
	p_args.add_argument('-p', '--patterns', required=False, type=forge.at_file, metavar='FILE.pts', help=hlp['p'])
	p_args.add_argument('-a', '--append-pts', required=False, action='store_true', help=hlp['a'])
	p_args.add_argument('-y', '--append-sym', required=False, type=forge.at_file, metavar='FILE.sym', help=hlp['y'])
	p_args.add_argument('-s', '--start', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['s'])
	p_args.add_argument('-x', '--offset', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['x'])
	p_args.add_argument('-u', '--hook', required=False, type=forge.at_hex, metavar='ADDRESS', help=hlp['u'])
	p_args.add_argument('-o', '--output', required=False, type=forge.at_path, metavar='DIRECTORY', help=hlp['o'])
	p_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return p_args.parse_check_arguments()


def main() -> None:
	start_time: datetime = datetime.now()
	args: dict[str, any] = parse_arguments()

	forge.set_logging_configuration(args['verbose'])

	start_ep1_portkit_work(args)

	time_elapsed: str = forge.format_timedelta(datetime.now() - start_time)
	logging.info(f'Time elapsed: "{time_elapsed}".')


if __name__ == '__main__':
	main()
