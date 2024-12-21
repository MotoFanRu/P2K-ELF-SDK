#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A PortKit Utility for building ElfPack v1.0 for Motorola phones on P2K platform.

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
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'C650_R365_G_0B.D3.08R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  True,        # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      forge.P2K_DIR_EP1_SYM / 'C650_R365_G_0B.D3.08R.sym', # Use prechached symbols file and disable any findings.
		'drive_patch':    'a'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R373_G_0E.30.49R': {
		'opts_all':       ['-DFTR_E1'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'E1_R373_G_0E.30.49R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R373_G_0E.30.79R': {
		'opts_all':       ['-DFTR_E1'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'E1_R373_G_0E.30.79R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R373_G_0E.30.DAR': {
		'opts_all':       ['-DFTR_E1'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    0x003137B4,  # ElfPack v1.0 patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'E1_R373_G_0E.30.DAR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452F_G_08.03.08R': {
		'opts_all':       ['-DFTR_K1'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x0151C350,  # ElfPack v1.0 patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'K1_R452F_G_08.03.08R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R3511_G_0A.52.45R_A': {
		'opts_all':       ['-DFTR_L6'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'L6_R3511_G_0A.52.45R_A.pat',
		'firmware':       forge.P2K_DIR_CG / 'L6_R3511_G_0A.52.45R_A.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R3443H1_G_0A.65.0BR': {
		'opts_all':       ['-DFTR_L6I'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'L6i_R3443H1_G_0A.65.0BR.pat',
		'firmware':       forge.P2K_DIR_CG / 'L6i_R3443H1_G_0A.65.0BR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R4513_G_08.B7.ACR_RB': {
		'opts_all':       ['-DFTR_L7'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x00CA01B0,  # ElfPack v1.0 patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'L7_R4513_G_08.B7.ACR_RB.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R4513_G_08.B7.E0R_RB': {
		'opts_all':       ['-DFTR_L7'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x00C90730,  # ElfPack v1.0 patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'L7_R4513_G_08.B7.E0R_RB.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452D_G_08.01.0AR': {
		'opts_all':       ['-DFTR_L7E'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x0021D340,  # ElfPack v1.0 patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'L7e_R452D_G_08.01.0AR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452J_G_08.22.05R': {
		'opts_all':       ['-DFTR_L9'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x0152E010,  # ElfPack v1.0 patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'L9_R452J_G_08.22.05R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R4441D_G_08.01.03R': {
		'opts_all':       ['-DFTR_V3I'],
		'addr_start':     0x100A0000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'V3i_R4441D_G_08.01.03R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R4515_G_08.BD.D3R': {
		'opts_all':       ['-DFTR_V3R'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'V3r_R4515_G_08.BD.D3R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R3512_G_0A.30.6CR': {
		'opts_all':       ['-DFTR_V235'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'V235_R3512_G_0A.30.6CR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R4513_G_08.B7.ACR': {
		'opts_all':       ['-DFTR_V360'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'V360_R4513_G_08.B7.ACR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'TRIPLETS_G_0B.09.72R': {
		'opts_all':       ['-DFTR_V600'],
		'addr_start':     0x10080000,  # Firmware start address.
		'addr_offset':    0x00C3C1B0,  # ElfPack v1.0 patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'V600_TRIPLETS_G_0B.09.72R.pat',
		'firmware':       forge.P2K_DIR_CG / 'V600_TRIPLETS_G_0B.09.72R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'a'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452B_G_08.02.0DR': {
		'opts_all':       ['-DFTR_Z3'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x0021D290,  # ElfPack v1.0 patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'Z3_R452B_G_08.02.0DR.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452F1_G_08.04.09R': {
		'opts_all':       ['-DFTR_Z3'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'Z3_R452F1_G_08.04.09R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R452H6_G_08.00.05R': {
		'opts_all':       ['-DFTR_Z3'],
		'addr_start':     0x10092000,  # Firmware start address.
		'addr_offset':    0x0021D290,  # ElfPack v1.0 patch address.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'Z3_R452H6_G_08.00.05R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      None,        # Use prechached symbols file and disable any findings.
		'drive_patch':    'c'          # Patch "/b/Elf/elfloader.lib" and "/b/Elf/auto.run" disk with this letter.
	},
	'R261171LD_U_99.51.06R': {
		'opts_all':       ['-DFTR_K3'],
		'addr_start':     0xA0080000,  # Firmware start address.
		'addr_offset':    None,        # ElfPack v1.0 patch address, will be calculated.
		'patterns':       forge.P2K_DIR_EP1_PAT / 'General.pat',
		'firmware':       forge.P2K_DIR_CG / 'K3_R261171LD_U_99.51.06R.smg',
		'func_inject':    'APP_SyncML_MainRegister',
		'use_afw_wraps':  False,       # Use AFW_CreateInternalQueuedEvPriv() for AFW_CreateInternalQueuedEvAux* funcs.
		'precached':      forge.P2K_DIR_EP1_SYM / 'K3_R261171LD_U_99.51.06R.sym', # Use prechached symbols file and disable any findings.
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
	return forge.libgen_apply_patches(patches, lib_sym, phone, firmware, 'EP1')


# Various generators.
def generate_lib_sym(p_i_f: Path, p_i_e: Path, p_o_l: Path, names_skip: list[str], patterns_add: list[str]) -> bool:
	if forge.check_files_if_exists([p_i_f, p_i_e]):
		with (p_i_f.open(mode='r') as i_f, p_i_e.open(mode='r') as i_s, p_o_l.open(mode='w', newline='\r\n') as o_l):
			o_l.write(f'{forge.ADS_SYM_FILE_HEADER}\n')
			o_l.write('# SYMDEFS ADS HEADER\n\n')
			for line in i_f.read().splitlines():
				address, mode, name = forge.split_and_validate_line(line)
				if (name is not None) and (name not in names_skip):
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


def generate_register_sym(combined_sym: Path, cgs_path: Path, register_func: str, pat: Path, sym: Path) -> bool:
	address: int = forge.get_function_address_from_sym_file(combined_sym, register_func)
	if address != 0x00000000:
		forge.pat_append(pat, 'Register', 'D', forge.int2hex_r(address))
		forge.pat_find(pat, cgs_path, 0x00000000, False, sym)
		if forge.ep1_libgen_model(sym, forge.LibrarySort.NAME) is not None:
			return True
	return False


# PortKit ARM v1.0 working flow.
def start_ep1_portkit_work(opts: dict[str, any]) -> bool:
	logging.info('Start building ElfPack v1.0 for Motorola P2K.')
	logging.info('')
	logging.info('Parameters:')
	forge.args_dump(opts)
	logging.info('')
	logging.info('Prepare PortKit environment.')
	forge.prepare_clean_output_directory(opts["output"], opts['clean'])
	logging.info('')

	val_combined_sym: Path = opts['output'] / 'Combined.sym'

	if not opts['precached']:
		logging.info('Finding SoC related functions from patterns.')
		val_lte1_pat: Path = forge.P2K_DIR_EP1_PAT / 'LTE.pat'
		val_lte2_pat: Path = forge.P2K_DIR_EP1_PAT / 'LTE2.pat'
		val_lte2_modern_pat: Path = forge.P2K_DIR_EP1_PAT / 'LTE2_Modern.pat'
		val_lte2_irom_sym: Path = forge.P2K_DIR_EP1_SYM / 'LTE2_IROM.sym'
		val_platform_sym: Path = opts['output'] / 'Platform.sym'
		val_functions_sym: Path = opts['output'] / 'Functions.sym'
		val_functions_modern_lte2: Path = opts['output'] / 'Functions_LTE2_Modern.sym'
		if opts['soc'] == 'LTE':
			forge.pat_find(val_lte1_pat, opts['fw_file'], opts['start'], False, val_platform_sym)
		elif opts['soc'] == 'LTE2':
			forge.pat_find(val_lte2_pat, opts['fw_file'], opts['start'], False, val_platform_sym)
			if forge.is_modern_lte2(opts['phone']):
				forge.pat_find(val_lte2_modern_pat, opts['fw_file'], opts['start'], False, val_functions_modern_lte2)
		else:
			val_functions_sym = val_combined_sym
			logging.warning('Unknown SoC platform, will skip generating platform symbols file.')
		logging.info('')

	if not opts['precached']:
		logging.info('Finding general functions from patterns.')
		forge.pat_find(opts['patterns'], opts['fw_file'], opts['start'], opts['ram_trans'], val_functions_sym)
		logging.info('')

	if not opts['precached']:
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
	else:
		val_combined_sym = opts['precached']

	if not opts['precached']:
		logging.info('Applying phone specific patches.')
		apply_patches(opts['phone'], opts['fw_name'], val_combined_sym)
		logging.info('')

	if not opts['precached']:
		logging.info('Validating combined symbols file.')
		if not forge.validate_sym_file(val_combined_sym):
			return False
		else:
			logging.info(f'The "{val_combined_sym}" sym file is validated.')
		logging.info('')

	val_register_sym: Path = opts['output'] / 'Register.sym'
	logging.info('Generating register symbols file.')
	val_register_pat: Path = opts['output'] / 'Register.pat'
	if not generate_register_sym(val_combined_sym, opts['fw_file'], opts['inject'], val_register_pat, val_register_sym):
		logging.error(f'Cannot generate "{val_register_pat}" and "{val_register_sym}" files.')
		return False
	logging.info('')

	logging.info('Generating system information C-source file.')
	val_system_info_c: Path = opts['output'] / 'SysInfo.c'
	val_system_info_o: Path = opts['output'] / 'SysInfo.o'
	generate_system_information_source(opts['phone'], opts['fw_name'], opts['soc'], val_system_info_c)
	logging.info('')

	logging.info('Compiling C-source files using ADS compiler.')
	forge.ep1_ads_tcc(val_system_info_c, val_system_info_o, True, ['-DEP1'])
	logging.info('')

	logging.info('Linking object files to binary.')
	val_object_path: Path = forge.P2K_DIR_EP1_OBJ
	if opts['new_obj']:
		val_object_path = forge.P2K_DIR_EP1_OBJ / 'new'
	# WARNING: Order is important here!
	val_link_objects: list[Path] = [
		val_object_path / 'AutoRun.o',
		val_object_path / 'ElfLoaderApp.o',
		val_object_path / 'ElfLoader.o',
		val_system_info_o,
		forge.P2K_DIR_EP1_LIB / 'libarm_small.a',
		val_combined_sym
	]
	if opts['use_afw_wraps']:
		val_link_objects.insert(3, Path(val_object_path / 'AFW_CreateInternalQueuedEv_Wrappers.o'))
	val_elfpack_elf: Path = opts['output'] / 'ElfPack.elf'
	val_elfpack_sym: Path = opts['output'] / 'ElfPack.sym'
	val_elfpack_bin: Path = opts['output'] / 'ElfPack.bin'
	if not forge.ep1_ads_armlink(val_link_objects, val_elfpack_elf, opts['address'], val_elfpack_sym):
		logging.error(f'Cannot link "{val_elfpack_elf}" executable file.')
		return False
	forge.ep1_ads_fromelf(val_elfpack_elf, val_elfpack_bin)
	logging.info('')

	logging.info('Patch resulting binaries.')
	d: str = forge.str2hex(opts["drive"])
	# file://b/Elf/elfloader.lib
	# Pattern: 66696C653A2F2F622F456C662F656C666C6F616465722E6C6962
	po1: str = '66696C653A2F2F622F456C662F656C666C6F616465722E6C6962'
	pn1: str = f'66696C653A2F2F{d}2F456C662F656C666C6F616465722E6C6962'
	forge.patch_binary_file_res(val_elfpack_bin, po1, pn1)
	# f.i.l.e.:././.b./.E.l.f./.a.u.t.o...r.u.n.
	# Pattern: 660069006C0065003A002F002F0062002F0045006C0066002F006100750074006F002E00720075006E00
	po2: str = '660069006C0065003A002F002F0062002F0045006C0066002F006100750074006F002E00720075006E00'
	pn2: str = f'660069006C0065003A002F002F00{d}002F0045006C0066002F006100750074006F002E00720075006E00'
	forge.patch_binary_file_res(val_elfpack_bin, po2, pn2)
	logging.info('')

	logging.info('Creating Flash&Backup 3 patches.')
	val_register_fpa: Path = opts['output'] / 'Register.fpa'
	val_elfpack_fpa: Path = opts['output'] / 'ElfPack.fpa'
	val_elfdir_fpa: Path = opts['output'] / 'ElfDirectory.fpa'
	val_result_fpa: Path = opts['output'] / 'Result.fpa'
	forge.bin2fpa(
		opts['fw_name'], 'Andy51', 'ElfPack v1.0', opts['offset'], val_elfpack_bin, val_elfpack_fpa, opts['fw_file']
	)
	generate_register_patch(
		opts['fw_name'], 'Andy51', 'ElfPack v1.0 Register patch',
		val_elfpack_sym, val_register_sym, val_register_fpa, opts['fw_file']
	)
	patches: list[Path] = [val_register_fpa, val_elfpack_fpa]
	if opts['directory']:
		desc: str = 'ElfPack v1.0 Directory patch'
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
	forge.unite_fpa_patches(opts['fw_name'], 'Andy51, EXL', 'Combined ElfPack v1.0 patch.', patches, val_result_fpa)
	logging.info('')

	logging.info('Creating ElfPack v1.0 library for Phone.')
	val_library_sym: Path = opts['output'] / 'Lib.sym'
	val_library_asm: Path = opts['output'] / 'Lib.asm'
	val_elfloader_lib: Path = opts['output'] / 'elfloader.lib'
	generate_lib_sym(
		val_combined_sym, val_elfpack_sym, val_library_sym,
		[opts['inject'], 'APP_CALC_MainRegister', '_region_table'],
		['Ldr', 'UtilLogStringData', 'namecmp', 'u_utoa', '_ll_cmpu']
	)
	functions, library_model = forge.ep1_libgen_model(val_library_sym, forge.LibrarySort.NAME)
	forge.ep1_libgen_asm(val_library_asm, library_model)
	forge.ep1_libgen_library(val_elfloader_lib, library_model, functions)
	forge.ep1_libgen_symbols(val_elfloader_lib, val_library_sym, forge.LibrarySort.NAME, opts['phone'], opts['fw_name'])
	logging.info('')

	logging.info('Compiling ElfPack v1.0 library for SDK.')
	val_library_obj: Path = opts['output'] / 'Lib.o'
	val_libstd_static_lib: Path = opts['output'] / 'libstd.a'
	forge.ep1_ads_armasm(val_library_asm, val_library_obj)
	forge.ep1_ads_armar([val_library_obj], val_libstd_static_lib)
	logging.info('')

	logging.info('ElfPack v1.0 building report:')
	logging.info('')
	logging.info('Important files:')
	logging.info(f'\t{val_elfloader_lib}\t-\tCompiled library for "{opts["phone"]}" on "{opts["fw_name"]}" firmware.')
	logging.info(f'\t{val_result_fpa}\t-\tGenerated ElfPack v1.0 combined patch for Flash&Backup.')
	logging.info('')
	logging.info('Developer files:')
	logging.info(f'\t{val_libstd_static_lib}\t-\tCompiled library for SDK.')
	logging.info(f'\t{val_library_sym}\t-\tGenerated library entities list.')
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
		opts['clean'] = args.clean
		opts['ram_trans'] = args.ram_trans
		opts['new_obj'] = args.new_obj
		opts['output'] = args.output
		opts['directory'] = args.directory
		opts['patterns'] = args.patterns if args.patterns else variants['patterns']
		opts['fw_file'] = args.firmware if args.firmware else variants['firmware']

		opts['start'] = args.start if args.start else variants['addr_start']
		opts['offset'] = args.offset if args.offset else variants['addr_offset']
		if not opts['offset']:
			opts['offset'] = forge.arrange16(forge.get_file_size(opts['fw_file']))
		opts['address'] = opts['start'] + opts['offset']
		opts['soc'] = forge.determine_soc(opts['start'])
		opts['phone'] = phone
		opts['fw_name'] = firmware

		opts['inject'] = variants['func_inject']

		opts['precached'] = variants['precached']
		opts['use_afw_wraps'] = variants['use_afw_wraps']

		opts['drive'] = variants['drive_patch']

		return opts


def parse_arguments() -> dict[str, any]:
	hlp: dict[str, str] = {
		'd': 'A PortKit Utility for building ElfPack v1.0 for Motorola phones on P2K platform, 15-Dec-2023',
		'c': 'clean output directory before processing',
		'r': 'resolve precached iRAM function addresses',
		'pf': 'phone and firmware, e.g. "E1_R373_G_0E.30.49R"',
		's': 'override start address of CG0+CG1 firmware (in HEX)',
		'p': 'override path to patterns file',
		'f': 'override path to CG0+CG1 firmware file',
		'o': 'output artifacts directory',
		'n': 'use new object files',
		't': 'generate patch with replacing "mixedmedia" to "Elf" directory',
		'g': 'override result patch offset in CG0+CG1 file (in HEX)',
		'v': 'verbose output'
	}
	epl: str = """examples:
	# Build ElfPack v1.0 and libraries to the phone/firmware (+'Elf' directory patch).
	python ep1_portkit.py -c -r -t -pf E1_R373_G_0E.30.49R -o build
	python ep1_portkit.py -c -r -t -pf E1_R373_G_0E.30.79R -o build
	python ep1_portkit.py -c -r -p ../../ep1/pat/E1_R373_G_0E.30.DAR.pat -pf E1_R373_G_0E.30.DAR -o build
	python ep1_portkit.py -c -r -t -pf K1_R452F_G_08.03.08R -o build
	python ep1_portkit.py -c -r -t -pf L6_R3511_G_0A.52.45R_A -o build
	python ep1_portkit.py -c -r -t -pf L6i_R3443H1_G_0A.65.0BR -o build
	python ep1_portkit.py -c -r -t -pf L7_R4513_G_08.B7.ACR_RB -o build
	python ep1_portkit.py -c -r -t -pf L7_R4513_G_08.B7.E0R_RB -o build
	python ep1_portkit.py -c -r -t -pf L7e_R452D_G_08.01.0AR -o build
	python ep1_portkit.py -c -r -t -pf L9_R452J_G_08.22.05R -o build
	python ep1_portkit.py -c -r -t -pf V3i_R4441D_G_08.01.03R -o build
	python ep1_portkit.py -c -r -t -pf V3r_R4515_G_08.BD.D3R -o build
	python ep1_portkit.py -c -r -t -pf V235_R3512_G_0A.30.6CR -o build
	python ep1_portkit.py -c -r -t -pf V360_R4513_G_08.B7.ACR -o build
	python ep1_portkit.py -c -r -t -pf V600_TRIPLETS_G_0B.09.72R -o build
	python ep1_portkit.py -c -r -t -pf Z3_R452B_G_08.02.0DR -o build
	python ep1_portkit.py -c -r -t -pf Z3_R452F1_G_08.04.09R -o build
	python ep1_portkit.py -c -r -t -pf Z3_R452H6_G_08.00.05R -o build
	python ep1_portkit.py -c -r -t -pf C650_R365_G_0B.D3.08R -o build
	python ep1_portkit.py -c -r -t -pf K3_R261171LD_U_99.51.06R -o build

	# Build ElfPack v1.0 and libraries to the phone/firmware using new object files.
	python ep1_portkit.py -c -r -t -n -pf E1_R373_G_0E.30.49R -o build

	# Build ElfPack v1.0 and libraries to the phone/firmware with patch offset override.
	python ep1_portkit.py -c -r -t -pf E1_R373_G_0E.30.49R -g 0x00C3C1B0 -o build
	"""
	parser_args: Args = Args(description=hlp['d'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-c', '--clean', required=False, action='store_true', help=hlp['c'])
	parser_args.add_argument('-r', '--ram-trans', required=False, action='store_true', help=hlp['r'])
	parser_args.add_argument('-pf', '--phone-fw', required=True, type=forge.at_pfw, metavar='PHONE_FW', help=hlp['pf'])
	parser_args.add_argument('-s', '--start', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['s'])
	parser_args.add_argument('-p', '--patterns', required=False, type=forge.at_file, metavar='FILE.pat', help=hlp['p'])
	parser_args.add_argument('-f', '--firmware', required=False, type=forge.at_ffw, metavar='FILE.smg', help=hlp['f'])
	parser_args.add_argument('-o', '--output', required=True, type=forge.at_path, metavar='DIRECTORY', help=hlp['o'])
	parser_args.add_argument('-n', '--new-obj', required=False, action='store_true', help=hlp['n'])
	parser_args.add_argument('-t', '--directory', required=False, action='store_true', help=hlp['t'])
	parser_args.add_argument('-g', '--offset', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['g'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	start_time: datetime = datetime.now()
	args: dict[str, any] = parse_arguments()

	forge.set_logging_configuration(args['verbose'])

	start_ep1_portkit_work(args)

	time_elapsed: str = forge.format_timedelta(datetime.now() - start_time)
	logging.info(f'Time elapsed: "{time_elapsed}".')


if __name__ == '__main__':
	main()
