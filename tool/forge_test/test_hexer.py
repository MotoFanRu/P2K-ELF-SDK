# forge_test/test_hexer.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import unittest

from forge import hex2int
from forge import int2hex
from forge import int2hex_r
from forge import arrange16
from forge import is_hex_string
from forge import normalize_hex_string


class TestHexer(unittest.TestCase):
	def test_hex2int(self):
		self.assertEqual(hex2int('0x00000000'), 0)
		self.assertEqual(hex2int('0x00000001'), 1)
		self.assertEqual(hex2int('0x10080000'), 268959744)
		self.assertEqual(hex2int('0xFFFFFFFF'), 4294967295)

		self.helper_test_hex2int('0x0')
		self.helper_test_hex2int('0x1')
		self.helper_test_hex2int('0x10')
		self.helper_test_hex2int('0x100')
		self.helper_test_hex2int('0x1000')
		self.helper_test_hex2int('0x10000')
		self.helper_test_hex2int('0x100000')
		self.helper_test_hex2int('0x1000000')
		# self.helper_test_hex2int('0x10000000')
		self.helper_test_hex2int('0')
		self.helper_test_hex2int('1')
		self.helper_test_hex2int('0001')
		self.helper_test_hex2int('1000')
		self.helper_test_hex2int('G00D')

	def helper_test_hex2int(self, hex_value):
		with self.assertRaises(ValueError) as context:
			hex2int(hex_value)

	def test_arrange16(self):
		self.assertEqual(int2hex(arrange16(0x10080000)), '0x10080010')
		self.assertEqual(int2hex(arrange16(0x10080001)), '0x10080010')
		self.assertEqual(int2hex(arrange16(0x10080002)), '0x10080010')
		self.assertEqual(int2hex(arrange16(0x10080004)), '0x10080010')
		self.assertEqual(int2hex(arrange16(0x10080008)), '0x10080010')
		self.assertEqual(int2hex(arrange16(0x10080009)), '0x10080010')
		self.assertEqual(int2hex(arrange16(0x1008000A)), '0x10080010')
		self.assertEqual(int2hex(arrange16(0x1008000C)), '0x10080010')
		self.assertEqual(int2hex(arrange16(0x1008000F)), '0x10080010')
		self.assertEqual(int2hex(arrange16(0x10080010)), '0x10080020')
		self.assertEqual(int2hex(arrange16(0x100800FF)), '0x10080100')

	def test_int2hex(self):
		self.assertEqual(int2hex(0), '0x00000000')
		self.assertEqual(int2hex(5), '0x00000005')
		self.assertEqual(int2hex(15), '0x0000000F')
		self.assertEqual(int2hex(16), '0x00000010')
		self.assertEqual(int2hex(4294967295), '0xFFFFFFFF')

	def test_int2hex_r(self):
		self.assertEqual(int2hex_r(0), '00000000')
		self.assertEqual(int2hex_r(5), '00000005')
		self.assertEqual(int2hex_r(15), '0000000F')
		self.assertEqual(int2hex_r(16), '00000010')
		self.assertEqual(int2hex_r(4294967295), 'FFFFFFFF')

	def test_is_hex_string(self):
		self.assertTrue(is_hex_string('0'))
		self.assertTrue(is_hex_string('9'))
		self.assertTrue(is_hex_string('A'))
		self.assertTrue(is_hex_string('a'))
		self.assertTrue(is_hex_string('F'))
		self.assertTrue(is_hex_string('f'))
		self.assertTrue(is_hex_string('0123456789abcdefABCDEF'))
		self.assertFalse(is_hex_string('G'))
		self.assertFalse(is_hex_string('g'))
		self.assertFalse(is_hex_string(' '))
		self.assertFalse(is_hex_string('0123456789abcdefABCDEFG'))

	def test_normalize_hex_string(self):
		self.assertEqual(normalize_hex_string('0123456789abcdefABCDEF'), '0123456789ABCDEFABCDEF')
		self.assertEqual(normalize_hex_string(' 0123456789abcdefABCDEF'), '0123456789ABCDEFABCDEF')
		self.assertEqual(normalize_hex_string(' 0123456789abcdefABCDEF'), '0123456789ABCDEFABCDEF')
		self.assertEqual(normalize_hex_string(' 0123456789abcdefABCDEF '), '0123456789ABCDEFABCDEF')
		self.assertEqual(normalize_hex_string(' 0123456789abcdefABCDEF '), '0123456789ABCDEFABCDEF')
		self.assertEqual(normalize_hex_string('0123456789ab cdefABCDEF'), None)
		self.assertEqual(normalize_hex_string('0123456789abGcdefABCDEF'), None)
