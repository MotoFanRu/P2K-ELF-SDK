# forge_test/test_firmware.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import unittest

from forge import determine_soc
from forge import parse_phone_firmware
from forge import parse_minor_major_firmware


class TestFirmware(unittest.TestCase):
	def test_parse_phone_firmware(self):
		self.assertEqual(parse_phone_firmware('E1_R373_G_0E.30.49R.smg'), ('E1', 'R373_G_0E.30.49R'))
		self.assertEqual(parse_phone_firmware('L7_R4513_G_08.B7.ACR_RB.smg'), ('L7', 'R4513_G_08.B7.ACR_RB'))
		self.assertEqual(parse_phone_firmware('V3i_R4441D_G_08.01.03R.smg'), ('V3i', 'R4441D_G_08.01.03R'))
		self.assertEqual(parse_phone_firmware('L7_R4513_G_08.B7.ACR_RB.smg'), ('L7', 'R4513_G_08.B7.ACR_RB'))
		self.assertEqual(parse_phone_firmware('L6_R3511_G_0A.52.45R_A.smg'), ('L6', 'R3511_G_0A.52.45R_A'))
		self.assertEqual(parse_phone_firmware('L6i_R3443H1_G_0A.65.0BR.smg'), ('L6i', 'R3443H1_G_0A.65.0BR'))
		self.assertEqual(parse_phone_firmware('V600_TRIPLETS_G_0B.09.72R.smg'), ('V600', 'TRIPLETS_G_0B.09.72R'))
		self.assertEqual(parse_phone_firmware('E1_R373_G_0E.30.49R.bin'), ('E1', 'R373_G_0E.30.49R'))

		self.helper_test_parse_phone_firmware('')
		self.helper_test_parse_phone_firmware('CG1')
		self.helper_test_parse_phone_firmware('CG1.bin')
		self.helper_test_parse_phone_firmware('CG1.smg')
		self.helper_test_parse_phone_firmware('E1')
		self.helper_test_parse_phone_firmware('E1.bin')
		self.helper_test_parse_phone_firmware('E1.smg')
		self.helper_test_parse_phone_firmware('E1_CG1.smg')
		self.helper_test_parse_phone_firmware('E1_CG1.bin')
		# self.helper_test_parse_phone_firmware('E1_R373_G_0E.30.49R.bin')

	def helper_test_parse_phone_firmware(self, firmware_filename):
		with self.assertRaises(ValueError):
			parse_phone_firmware(firmware_filename)

	def test_determine_soc(self):
		self.assertEqual(determine_soc(0x10080000), 'LTE')
		self.assertEqual(determine_soc(0x10092000), 'LTE2')
		self.assertEqual(determine_soc(0x100A0000), 'LTE2')
		self.assertEqual(determine_soc(0x18500000), 'Unknown')

	def test_parse_minor_major_firmware(self):
		self.assertEqual(parse_minor_major_firmware('R373_G_0E.30.49R'), ('R373_G_0E.30', '49R'))
		self.assertEqual(parse_minor_major_firmware('R4513_G_08.B7.ACR_RB'), ('R4513_G_08.B7', 'ACR_RB'))
		self.assertEqual(parse_minor_major_firmware('R4441D_G_08.01.03R'), ('R4441D_G_08.01', '03R'))
		self.assertEqual(parse_minor_major_firmware('R4513_G_08.B7.ACR_RB'), ('R4513_G_08.B7', 'ACR_RB'))
		self.assertEqual(parse_minor_major_firmware('R3511_G_0A.52.45R_A'), ('R3511_G_0A.52', '45R_A'))
		self.assertEqual(parse_minor_major_firmware('R3443H1_G_0A.65.0BR'), ('R3443H1_G_0A.65', '0BR'))
		self.assertEqual(parse_minor_major_firmware('TRIPLETS_G_0B.09.72R'), ('TRIPLETS_G_0B.09', '72R'))
