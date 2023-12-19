# forge_test/test_hexer.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

import unittest

from forge import hex2int
from forge import int2hex
from forge import hex2int_r
from forge import int2hex_r
from forge import arrange16
from forge import is_hex_string
from forge import normalize_hex_string
from forge import normalize_hex_address


class TestHexer(unittest.TestCase):
	def test_hex2int(self) -> None:
		self.assertEqual(hex2int('0x00000000'), 0)
		self.assertEqual(hex2int('0x00000001'), 1)
		self.assertEqual(hex2int('0x10080000'), 268959744)
		self.assertEqual(hex2int('0xFFFFFFFF'), 4294967295)
		self.assertEqual(hex2int('0x1008', 4), 4104)

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
		# self.helper_test_hex2int('0x1000', 4)

	def helper_test_hex2int(self, hex_value: str, size: int = 8) -> None:
		with self.assertRaises(ValueError):
			hex2int(hex_value, size)

	def test_hex2int_r(self) -> None:
		self.assertEqual(hex2int_r('00000000'), 0)
		self.assertEqual(hex2int_r('00000001'), 1)
		self.assertEqual(hex2int_r('10080000'), 268959744)
		self.assertEqual(hex2int_r('FFFFFFFF'), 4294967295)

		self.helper_test_hex2int_r('0')
		self.helper_test_hex2int_r('1')
		self.helper_test_hex2int_r('10')
		self.helper_test_hex2int_r('100')
		self.helper_test_hex2int_r('1000')
		self.helper_test_hex2int_r('10000')
		self.helper_test_hex2int_r('100000')
		self.helper_test_hex2int_r('1000000')
		# self.helper_test_hex2int_r('10000000')
		self.helper_test_hex2int_r('0')
		self.helper_test_hex2int_r('1')
		self.helper_test_hex2int_r('0001')
		self.helper_test_hex2int_r('1000')
		self.helper_test_hex2int_r('G00D')

	def helper_test_hex2int_r(self, hex_value: str) -> None:
		with self.assertRaises(ValueError):
			hex2int_r(hex_value)

	def test_arrange16(self) -> None:
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

	def test_int2hex(self) -> None:
		self.assertEqual(int2hex(0), '0x00000000')
		self.assertEqual(int2hex(5), '0x00000005')
		self.assertEqual(int2hex(15), '0x0000000F')
		self.assertEqual(int2hex(16), '0x00000010')
		self.assertEqual(int2hex(4294967295), '0xFFFFFFFF')
		self.assertEqual(int2hex(0, 4), '0x0000')
		self.assertEqual(int2hex(5, 4), '0x0005')
		self.assertEqual(int2hex(15, 4), '0x000F')
		self.assertEqual(int2hex(16, 4), '0x0010')
		self.assertEqual(int2hex(4294967295, 4), '0xFFFFFFFF')

	def test_int2hex_r(self) -> None:
		self.assertEqual(int2hex_r(0), '00000000')
		self.assertEqual(int2hex_r(5), '00000005')
		self.assertEqual(int2hex_r(15), '0000000F')
		self.assertEqual(int2hex_r(16), '00000010')
		self.assertEqual(int2hex_r(4294967295), 'FFFFFFFF')

	def test_is_hex_string(self) -> None:
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

	def test_normalize_hex_string(self) -> None:
		self.assertEqual(normalize_hex_string('0123456789abcdefABCDEF'), '0123456789ABCDEFABCDEF')
		self.assertEqual(normalize_hex_string(' 0123456789abcdefABCDEF'), '0123456789ABCDEFABCDEF')
		self.assertEqual(normalize_hex_string(' 0123456789abcdefABCDEF'), '0123456789ABCDEFABCDEF')
		self.assertEqual(normalize_hex_string(' 0123456789abcdefABCDEF '), '0123456789ABCDEFABCDEF')
		self.assertEqual(normalize_hex_string(' 0123456789abcdefABCDEF '), '0123456789ABCDEFABCDEF')
		self.assertEqual(normalize_hex_string('0123456789ab cdefABCDEF'), None)
		self.assertEqual(normalize_hex_string('0123456789abGcdefABCDEF'), None)

	def test_normalize_hex_address(self) -> None:
		self.assertEqual(normalize_hex_address('10', True), '00000010')
		self.assertEqual(normalize_hex_address('10', False), '0x00000010')
		self.assertEqual(normalize_hex_address('10A', True), '0000010A')
		self.assertEqual(normalize_hex_address('10A', False), '0x0000010A')
		self.assertEqual(normalize_hex_address('0123456789abcdefABCDEF', True), None)
		self.assertEqual(normalize_hex_address('0123456789abcdefABCDEF', False), None)
