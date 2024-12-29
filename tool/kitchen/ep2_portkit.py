#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
A PortKit Utility for building ElfPack v2.0 for Motorola phones on P2K platform.

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
FUNC_REGISTER: str = 'elfpackEntry_ven'

EP2_PFW_VARIANTS: dict[str, dict[str, any]] = {
	'R373_G_0E.30.49R': {
		'fw':    forge.P2K_DIR_CG / 'E1_R373_G_0E.30.49R.smg',
		'o_gen': ['-DDEBUG', '-DFTR_E1', '-DFTR_PHONE_NAME="E1"', '-DFTR_PHONE_PLATFORM="LTE"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10080000,  # Firmware start address.
		'a_ep2': 0x00C73000,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0025A434,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see `UpdateDisplayInjection.asm` listing.
		'a_upd': 0x00028DF0,  # Update Display patch address,
		# Ram_398_l7 D [80A842B0D1062006+0x26]+0x10.
		# DataLogger block 0x12200000-0x122008CC (Andy51).
		'a_ram': 0x124FD320   # At least ~0x400 free bytes block in RAM, alternate: 0x122008E0.
	},
	'R373_G_0E.30.79R': {
		'fw':    forge.P2K_DIR_CG / 'E1_R373_G_0E.30.79R.smg',
		'o_gen': ['-DDEBUG', '-DFTR_E1', '-DFTR_PHONE_NAME="E1"', '-DFTR_PHONE_PLATFORM="LTE"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10080000,  # Firmware start address.
		'a_ep2': 0x00C73000,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0025CC50,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x00029914,  # Update Display patch address.
		# Ram_398_l7 D [80A842B0D1062006+0x26]+0x10.
		'a_ram': 0x12502538   # At least ~0x400 free bytes block in RAM.
	},
	'R373_G_0E.30.DAR': {
		'fw':    forge.P2K_DIR_CG / 'E1_R373_G_0E.30.DAR.smg',
		'o_gen': ['-DDEBUG', '-DFTR_E1', '-DFTR_PHONE_NAME="E1"', '-DFTR_PHONE_PLATFORM="LTE"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10080000,  # Firmware start address.
		'a_ep2': 0x002C47C0,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x003190B4,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x00315C94,  # Update Display patch address.
		# Ram_398_l7 D [80A842B0D1062006+0x26]+0x10.
		'a_ram': 0x12318C5C   # At least ~0x400 free bytes block in RAM.
	},
	'R452F_G_08.03.08R': {
		'fw':    forge.P2K_DIR_CG / 'K1_R452F_G_08.03.08R.smg',
		'o_gen': ['-DDEBUG', '-DFTR_L7E', '-DFTR_PHONE_NAME="K1"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_NONE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10092000,  # Firmware start address.
		'a_ep2': 0x01530000,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x011EB174,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x00244310,  # Update Display patch address.
		# Ram_l7e D [7FFF0000011E00000122+0x0A], probably wrong alternative 0x1402B2A4 value (Andy51).
		# Ram_l7e D [201490002000900190029003+0x01E8]+28.
		'a_ram': 0x14501210   # At least ~0x400 free bytes block in RAM.
	},
	'R4513_G_08.B7.ACR_RB': {
		'fw':    forge.P2K_DIR_CG / 'L7_R4513_G_08.B7.ACR_RB.smg',
		'o_gen': ['-DDEBUG', '-DFTR_L7', '-DFTR_PHONE_NAME="L7"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10092000,  # Firmware start address.
		'a_ep2': 0x00C8D630,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0027DD30,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		# loc_noname A 1 E1A02007E1A01005+0x8.
		'a_upd': 0x00014568,  # Update Display patch address.
		# Pattern: Ram_398_l7 D [80A842B0D1062006+0x26]+0x10.
		'a_ram': 0x124EBBA0   # At least ~0x400 free bytes block in RAM.
	},
	'R4513_G_08.B7.E0R_RB': {
		'fw':    forge.P2K_DIR_CG / 'L7_R4513_G_08.B7.E0R_RB.smg',
		'o_gen': ['-DDEBUG', '-DFTR_L7', '-DFTR_PHONE_NAME="L7"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10092000,  # Firmware start address.
		'a_ep2': 0x00CD0D00,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0027E6CC,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		# loc_noname A 1 E1A02007E1A01005+0x8.
		'a_upd': 0x000145BC,  # Update Display patch address.
		# Ram_398_l7 D [80A842B0D1062006+0x26]+0x10.
		'a_ram': 0x124EC694  # At least ~0x400 free bytes block in RAM.
	},
	'R452D_G_08.01.0AR': {
		'fw':    forge.P2K_DIR_CG / 'L7e_R452D_G_08.01.0AR.smg',
		'o_gen': ['-DDEBUG', '-DFTR_L7E', '-DFTR_PHONE_NAME="L7e"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_NONE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10092000,  # Firmware start address.
		'a_ep2': 0x01430000,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x01253FEC,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x0028E148,  # Update Display patch address.
		# Ram_l7e D [7FFF0000011E00000122+0x0A], probably wrong alternative 0x145C96C8 value (Andy51).
		# Ram_l7e D [2014900020009001900290039004480E+0x2A]+6C.
		'a_ram': 0x140737B0  # At least ~0x400 free bytes block in RAM.
	},
	'R452J_G_08.22.05R': {
		'fw':    forge.P2K_DIR_CG / 'L9_R452J_G_08.22.05R.smg',
		'o_gen': ['-DDEBUG', '-DFTR_L9', '-DFTR_PHONE_NAME="L9"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_NONE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10092000,  # Firmware start address.
		'a_ep2': 0x0153119C,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0121E0CC,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x00236C6C,  # Update Display patch address.
		# 0x1451C1C8 0x3e9 uis_data_logger_buffer (Andy51).
		# Ram_l7e D [7FFF0000011E00000122+0x0A].
		'a_ram': 0x1451C1C8   # At least ~0x400 free bytes block in RAM.
	},
	'R4441D_G_08.01.03R': {
		'fw':    forge.P2K_DIR_CG / 'V3i_R4441D_G_08.01.03R.smg',
		'o_gen': ['-DDEBUG', '-DFTR_V3I', '-DFTR_PHONE_NAME="V3i"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x100A0000,  # Firmware start address.
		'a_ep2': 0x00CDAE30,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0025C460,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x00014864,  # Update Display patch address.
		# Ram_v3i D [80A842B0D1312006+0x1E]+0x10.
		'a_ram': 0x1446428C   # At least ~0x400 free bytes block in RAM.
	},
	'R4515_G_08.BD.D3R': {
		'fw':    forge.P2K_DIR_CG / 'V3r_R4515_G_08.BD.D3R.smg',
		'o_gen': ['-DDEBUG', '-DFTR_V3R', '-DFTR_PHONE_NAME="V3r"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10092000,  # Firmware start address.
		'a_ep2': 0x00C60F10,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0025E6D8,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x0001CF2C,  # Update Display patch address.
		# Ram_v3r D [80A842B0D12E2006+0x1C]+0x10.
		'a_ram': 0x144C7C68   # At least ~0x400 free bytes block in RAM.
	},
	'R4513_G_08.B7.ACR': {
		'fw':    forge.P2K_DIR_CG / 'V360_R4513_G_08.B7.ACR.smg',
		'o_gen': ['-DDEBUG', '-DFTR_V360', '-DFTR_PHONE_NAME="V360"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10092000,  # Firmware start address.
		'a_ep2': 0x00C8D000,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0027DD30,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		# loc_noname A 1 E1A02007E1A01005+0x8.
		'a_upd': 0x00014568,  # Update Display patch address.
		# Ram_398_l7 D [80A842B0D1062006+0x26]+0x18.
		'a_ram': 0x124EB7B0   # At least ~0x400 free bytes block in RAM.
	},
	'TRIPLETS_G_0B.09.72R': {
		'fw':    forge.P2K_DIR_CG / 'V600_TRIPLETS_G_0B.09.72R.smg',
		'o_gen': ['-DDEBUG', '-DFTR_V600', '-DFTR_PHONE_NAME="V600"', '-DFTR_PHONE_PLATFORM="LTE"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_THREE_POLE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10080000,  # Firmware start address.
		'a_ep2': 0x00D50000,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x00391148,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x00AC5248,  # Update Display patch address.
		# Not sure about this one address, but looks like it works. Anyway EP2 for V600 is useless (EXL).
		'a_ram': 0x122008E0   # At least ~0x400 free bytes block in RAM, alternate: 0x122008E0.
	},
	'R452B_G_08.02.0DR': {
		'fw':    forge.P2K_DIR_CG / 'Z3_R452B_G_08.02.0DR.smg',
		'o_gen': ['-DDEBUG', '-DFTR_L7E', '-DFTR_PHONE_NAME="Z3"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_NONE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10092000,  # Firmware start address.
		'a_ep2': 0x01456000,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0118AD54,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x002E6328,  # Update Display patch address.
		# Ram_l7e D [7FFF0000011E00000122+0x0A], probably wrong alternative 0x145D5200 value (Andy51).
		# Ram_l7e D [2014900020009001900290039004480E+0x2A]+6C, 0x14073328 or 0x1407332C but probably first (Andy51).
		'a_ram': 0x14073328   # At least ~0x400 free bytes block in RAM.
	},
	'R452F1_G_08.04.09R': {
		'fw':    forge.P2K_DIR_CG / 'Z3_R452F1_G_08.04.09R.smg',
		'o_gen': ['-DDEBUG', '-DFTR_L7E', '-DFTR_PHONE_NAME="Z3"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_NONE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10092000,  # Firmware start address.
		'a_ep2': 0x0153C000,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0118B0C8,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x002CD2B0,  # Update Display patch address.
		# Ram_l7e D [7FFF0000011E00000122+0x0A], probably wrong alternative 0x1453EC28 value (Andy51).
		# Ram_l7e D [14??????00003E580000FFFF]+0x4.
		'a_ram': 0x14076374   # At least ~0x400 free bytes block in RAM.
	},
	'R452H6_G_08.00.05R': {
		'fw':    forge.P2K_DIR_CG / 'Z3_R452H6_G_08.00.05R.smg',
		'o_gen': ['-DDEBUG', '-DFTR_L7E', '-DFTR_PHONE_NAME="Z3"', '-DFTR_PHONE_PLATFORM="LTE2"'],
		'o_kbd': ['-DFTR_KEYPAD_TYPE=KP_NONE', '-DFTR_NOAUTORUN_KEY=KEY_0', '-DFTR_FAST_KEY=KEY_FAST_ACCESS'],
		'o_dbg': ['-DDEBUG', '-DLOG_TO_FILE', '-DDUMP_ELF', '-DFTR_LOG_FILE_URI=L"file://a/ep2.log"'],
		'a_fw':  0x10092000,  # Firmware start address.
		'a_ep2': 0x0145B000,  # ElfPack v2.0 offset address.
		# APP_SYNCMLMGR_MainRegister + 1.
		'a_reg': 0x0118B0FC,  # Register patch address.
		# DAL_WriteDisplayRegion xrefs, see UpdateDisplayInjection.asm listing.
		'a_upd': 0x002E6228,  # Update Display patch address.
		# Ram_l7e D [7FFF0000011E00000122+0x0A], probably wrong alternative 0x145D5200 value (Andy51).
		# Ram_l7e D [2014900020009001900290039004480E+0x2A]+6C, 0x14073328 or 0x1407332C but probably first (Andy51).
		'a_ram': 0x14073328   # At least ~0x400 free bytes block in RAM.
	}
}


# Patches.
def get_additional_pfw_patches(phone: str, firmware: str) -> list[Path]:
	patches: list[Path] = []
	fpa_dir: Path = forge.P2K_DIR_EP2_FPA
	if phone == 'K1':
		if firmware == 'R452F_G_08.03.08R':
			patches.append(fpa_dir / 'K1_08R_Drop_UIS_LogString.fpa')
	elif phone == 'L7e':
		if firmware == 'R452D_G_08.01.0AR':
			# Change in EP1 library if apply this patch:
			#	`0x10C5964A T PFprintf` => `0x10C5964C T PFprintf`
			# patches.append(fpa_dir / 'L7e_0AR_Drop_Additional_Logs.fpa')
			patches.append(fpa_dir / 'L7e_0AR_Drop_UIS_LogString.fpa')
	elif phone == 'L9':
		if firmware == 'R452J_G_08.22.05R':
			patches.append(fpa_dir / 'L9_05R_Drop_UIS_LogString.fpa')
	elif phone == 'Z3':
		if firmware == 'R452B_G_08.02.0DR':
			patches.append(fpa_dir / 'Z3_0DR_Drop_UIS_LogString.fpa')
		elif firmware == 'R452F1_G_08.04.09R':
			patches.append(fpa_dir / 'Z3_09R_Drop_UIS_LogString.fpa')
		elif firmware == 'R452H6_G_08.00.05R':
			patches.append(fpa_dir / 'Z3_05R_Drop_UIS_LogString.fpa')
	return patches


# Various generators.
def generate_register_patch(fw: str, author: str, desc: str, p_r: Path, reg_address: int, p_p: Path, cg: Path) -> bool:
	if forge.check_files_if_exists([p_r]):
		hex_data: str = forge.int2hex_r(forge.get_function_address_from_sym_file(p_r, FUNC_REGISTER))
		forge.hex2fpa(fw, author, desc, reg_address, hex_data, p_p, cg)
		return True
	return False


# PortKit ARM v2.0 working flow.
def start_ep2_portkit_work(opts: dict[str, any]) -> bool:
	logging.info('Start building ElfPack v2.0 for Motorola P2K.')
	logging.info('')
	logging.info('Parameters:')
	forge.args_dump(opts)
	logging.info('')
	logging.info('Prepare PortKit environment.')
	forge.prepare_clean_output_directory(opts["output"], opts['clean'])
	logging.info('')

	logging.info('Compiling ASM-source files using ADS compiler.')
	asm_sources: list[tuple[str, bool]] = [
		('dlopen.asm', False),
		('ldrUnloadElf.asm', False),
		('patchlib.asm', False),
		('veneers.asm', False),
		('UpdateDisplayInjection.asm', True)
	]
	for asm_source, arm_mode in asm_sources:
		p_asm: Path = forge.P2K_DIR_EP2_SRC / asm_source
		p_o: Path = opts['output'] / (asm_source + '.o')
		if not forge.ep1_ads_armasm(p_asm, p_o, arm_mode):
			return False
	logging.info('')

	logging.info('Compiling C-source files using ADS compiler.')
	c_sources: list[str] = [
		'API.c',
		'autorun.c',
		'config.c',
		'console.c',
		'dd.c',
		'dispCallBacks.c',
		'elfloaderApp.c',
		'font.c',
		'loadCommon.c',
		'loadDefLib.c',
		'loadElf.c',
		'loadLibrary.c',
		'logo.c',
		'lolvl.c',
		'palette.c',
		'parser.c',
		'utils.c'
	]
	for c_source in c_sources:
		p_c: Path = forge.P2K_DIR_EP2_SRC / c_source
		p_o: Path = opts['output'] / (c_source + '.o')
		if not forge.ep1_ads_tcc(p_c, p_o, True, opts['flags']):
			return False
	logging.info('')

	logging.info('Create linker files from template.')
	val_scatter_template: Path = forge.P2K_DIR_EP2_TPL / 'scatter.tpl'
	val_scatter_file: Path = opts['output'] / 'scat.scf'
	forge.patch_text_file_template(
		val_scatter_template, val_scatter_file, {
			'%addr_disp%': forge.int2hex(opts['addr_disp']),
			'%addr_main%': forge.int2hex(opts['addr_main']),
			'%addr_block%': forge.int2hex(opts['addr_block']),
		}
	)
	val_viafile_template: Path = forge.P2K_DIR_EP2_TPL / 'viafile.tpl'
	val_viafile_file: Path = opts['output'] / 'viafile.txt'
	forge.patch_text_file_template(
		val_viafile_template, val_viafile_file, {'%info_file%': str(opts['output'] / 'info.txt')}
	)
	logging.info('')

	logging.info('Generate link symbols file.')
	val_link_def: Path = forge.P2K_DIR_EP2_DEF / 'ElfLoaderLink.def'
	val_link_sym: Path = opts['output'] / 'Link.sym'
	forge.ep2_libgen_chunk_sym(
		opts['sym'], val_link_sym, forge.LibrarySort.NAME, forge.ep2_libgen_names_sym(val_link_def, True), opts['pfw']
	)
	logging.info('')

	logging.info('Linking object files to binary.')
	val_elfpack_elf: Path = opts['output'] / 'ElfPack.elf'
	val_elfpack_sym: Path = opts['output'] / 'ElfPack.sym'
	val_elfpack_bin: Path = opts['output']
	val_elfpack_bin_p: Path = opts['output'] / 'patch.bin'
	val_elfpack_bin_d: Path = opts['output'] / 'UpdDisplInjection.bin'
	lfs: list[Path] = []
	for asm_source, mode in asm_sources:
		lfs.append(opts['output'] / (asm_source + '.o'))
	for c_source in c_sources:
		lfs.append(opts['output'] / (c_source + '.o'))
	lfs.append(forge.P2K_DIR_EP1_LIB / 'libarm.a')
	lfs = forge.sort_paths_by_filename(lfs, True)
	lfs.append(val_link_sym)
	if not forge.ep1_ads_armlink_scatter(lfs, val_elfpack_elf, val_scatter_file, val_viafile_file, val_elfpack_sym):
		return False
	forge.ep1_ads_fromelf(val_elfpack_elf, val_elfpack_bin)
	if not forge.check_files_if_exists([val_elfpack_bin_p, val_elfpack_bin_d, val_elfpack_sym]):
		return False
	logging.info('')

	logging.info('Creating Flash&Backup 3 patches.')
	val_result_fpa: Path = opts['output'] / 'Result.fpa'
	val_register_fpa: Path = opts['output'] / 'Register.fpa'
	val_display_fpa: Path = opts['output'] / 'Display.fpa'
	val_elfpack_fpa: Path = opts['output'] / 'ElfPack.fpa'
	val_elfdir_fpa: Path = opts['output'] / 'Directory.fpa'
	authors: str = 'Andy51, tim_apple'
	desc_r: str = 'ElfPack v2.0 Register patch'
	desc_d: str = 'ElfPack v2.0 Display injection patch'
	desc_e: str = 'ElfPack v2.0'
	generate_register_patch(
		opts['fw_name'], authors, desc_r, val_elfpack_sym, opts['register'], val_register_fpa, opts['fw_file']
	)
	forge.bin2fpa(
		opts['fw_name'], authors, desc_d, opts['display'], val_elfpack_bin_d, val_display_fpa, opts['fw_file']
	)
	forge.bin2fpa(
		opts['fw_name'], authors, desc_e, opts['offset'], val_elfpack_bin_p, val_elfpack_fpa, opts['fw_file']
	)
	patches: list[Path] = [val_register_fpa, val_display_fpa, val_elfpack_fpa]
	if opts['directory']:
		desc: str = 'ElfPack v2.0 Directory patch'
		logging.info(f'Generate {desc} for "ringtone" => "Elf2" directory: "{val_elfdir_fpa}".')
		po3: str = (
			'00720069006E00670074006F006E0065'
			'00000000000000000000000000000000'
			'00000000000000000000000000000000'
			'00000000000000000000000000000000'
			'0000000000000000000000160002'
		)
		pn3: str = (
			'0045006C006600320000000000000000'
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
	all_authors: str = 'Andy51, tim_apple, EXL, fkcoder'
	patches.extend(get_additional_pfw_patches(opts['phone'], opts['fw_name']))
	forge.unite_fpa_patches(opts['fw_name'], all_authors, 'Combined ElfPack v2.0 patch.', patches, val_result_fpa)
	logging.info('')

	logging.info('ElfPack v2.0 building report.')
	logging.info('')
	logging.info(f'Compiled ElfPack v2.0 for "{opts["phone"]}" on "{opts["fw_name"]}" firmware.')
	logging.info('')
	logging.info('Important files:')
	logging.info(f'\t{val_result_fpa}\t-\tGenerated ElfPack v2.0 combined patch for Flash&Backup 3.')
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

		opts['pfw'] = args.phone_fw
		phone, firmware = args.phone_fw
		variants: dict[str, any] = EP2_PFW_VARIANTS.get(firmware, None)
		if not variants:
			self.error(f'unknown {phone} phone and {firmware} firmware')
		sym_source_file: Path = forge.ep1_libgen_get_library_sym(args.phone_fw)
		if not forge.check_files_if_exists([sym_source_file]):
			self.error(f'cannot find {sym_source_file} file with entity addresses')

		opts['verbose'] = args.verbose
		opts['clean'] = args.clean
		opts['output'] = args.output
		opts['debug'] = args.debug
		opts['directory'] = args.directory

		opts['start'] = args.start if args.start else variants['a_fw']
		opts['offset'] = args.offset if args.offset else variants['a_ep2']
		opts['register'] = args.register if args.register else variants['a_reg']
		opts['display'] = args.display if args.display else variants['a_upd']
		opts['addr_block'] = args.block if args.block else variants['a_ram']
		opts['fw_file'] = args.firmware if args.firmware else variants['fw']

		flags: list[str] = []
		# ElfPack v2.0 must be compiled with ElfPack v1.0 define `EP1`, for some event constants in `ev_codes1.h` file.
		flags.append('-DEP1')
		flags.extend(variants['o_gen'])
		flags.extend(variants['o_kbd'])
		if args.debug:
			flags.extend(variants['o_dbg'])
		opts['flags'] = flags

		opts['soc'] = forge.determine_soc(opts['start'])
		opts['phone'] = phone
		opts['fw_name'] = firmware
		opts['sym'] = sym_source_file

		opts['addr_main'] = opts['start'] + opts['offset']
		opts['addr_disp'] = opts['start'] + opts['display']

		return opts


def parse_arguments() -> dict[str, any]:
	hlp: dict[str, str] = {
		'd': 'A PortKit Utility for building ElfPack v2.0 for Motorola phones on P2K platform, 15-Dec-2023',
		'c': 'clean output directory before processing',
		'pf': 'phone and firmware, e.g. "E1_R373_G_0E.30.49R"',
		's': 'override start address of CG0+CG1 firmware (in HEX)',
		'g': 'override result patch offset in CG0+CG1 file (in HEX)',
		'r': 'override register address (in HEX)',
		'j': 'override display inject address (in HEX)',
		'b': 'override block RAM address (in HEX)',
		'f': 'override path to CG0+CG1 firmware file',
		'o': 'output artifacts directory',
		'db': 'debug build of ElfPack v2.0',
		't': 'generate patch with replacing "ringtone" to "Elf" directory',
		'v': 'verbose output'
	}
	epl: str = """examples:
	# Build ElfPack v2.0 to the phone/firmware using source code (+'Elf' directory patch).
	python ep2_portkit.py -c -t -pf E1_R373_G_0E.30.49R -o build
	python ep2_portkit.py -c -t -pf E1_R373_G_0E.30.79R -o build
	python ep2_portkit.py -c -pf E1_R373_G_0E.30.DAR -o build
	python ep2_portkit.py -c -t -pf K1_R452F_G_08.03.08R -o build
	python ep2_portkit.py -c -t -pf L7_R4513_G_08.B7.ACR_RB -o build
	python ep2_portkit.py -c -t -pf L7_R4513_G_08.B7.E0R_RB -o build
	python ep2_portkit.py -c -t -pf L7e_R452D_G_08.01.0AR -o build
	python ep2_portkit.py -c -t -pf L9_R452J_G_08.22.05R -o build
	python ep2_portkit.py -c -t -pf V3i_R4441D_G_08.01.03R -o build
	python ep2_portkit.py -c -t -pf V3r_R4515_G_08.BD.D3R -o build
	python ep2_portkit.py -c -t -pf V360_R4513_G_08.B7.ACR -o build
	python ep2_portkit.py -c -t -pf V600_TRIPLETS_G_0B.09.72R -o build
	python ep2_portkit.py -c -t -pf Z3_R452B_G_08.02.0DR -o build
	python ep2_portkit.py -c -t -pf Z3_R452F1_G_08.04.09R -o build
	python ep2_portkit.py -c -t -pf Z3_R452H6_G_08.00.05R -o build

	# Build ElfPack v2.0 to the phone/firmware using source code with patch offset override.
	python ep2_portkit.py -c -t -pf E1_R373_G_0E.30.49R -g 0x00C3C1B0 -o build
	"""
	parser_args: Args = Args(description=hlp['d'], epilog=epl, formatter_class=argparse.RawDescriptionHelpFormatter)
	parser_args.add_argument('-c', '--clean', required=False, action='store_true', help=hlp['c'])
	parser_args.add_argument('-pf', '--phone-fw', required=True, type=forge.at_pfw, metavar='PHONE_FW', help=hlp['pf'])
	parser_args.add_argument('-s', '--start', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['s'])
	parser_args.add_argument('-g', '--offset', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['g'])
	parser_args.add_argument('-r', '--register', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['r'])
	parser_args.add_argument('-j', '--display', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['j'])
	parser_args.add_argument('-b', '--block', required=False, type=forge.at_hex, metavar='OFFSET', help=hlp['b'])
	parser_args.add_argument('-f', '--firmware', required=False, type=forge.at_ffw, metavar='FILE.smg', help=hlp['f'])
	parser_args.add_argument('-o', '--output', required=True, type=forge.at_path, metavar='DIRECTORY', help=hlp['o'])
	parser_args.add_argument('-d', '--debug', required=False, action='store_true', help=hlp['db'])
	parser_args.add_argument('-t', '--directory', required=False, action='store_true', help=hlp['t'])
	parser_args.add_argument('-v', '--verbose', required=False, action='store_true', help=hlp['v'])
	return parser_args.parse_check_arguments()


def main() -> None:
	start_time: datetime = datetime.now()
	args: dict[str, any] = parse_arguments()

	forge.set_logging_configuration(args['verbose'])

	start_ep2_portkit_work(args)

	time_elapsed: str = forge.format_timedelta(datetime.now() - start_time)
	logging.info(f'Time elapsed: "{time_elapsed}".')


if __name__ == '__main__':
	main()
