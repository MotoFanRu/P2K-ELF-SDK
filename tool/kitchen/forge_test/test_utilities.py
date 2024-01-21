# forge_test/test_utilities.py
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

from datetime import datetime

from forge import format_timedelta
from forge import chop_str
from forge import is_string_filled_by_character


class TestUtilities(unittest.TestCase):
	def test_format_timedelta(self) -> None:
		now = datetime.now()
		self.assertEqual(len(format_timedelta(datetime.now() - now)), len('00:00:00.000'))

	def test_chop_string_to_16_symbols(self) -> None:
		self.assertEqual(chop_str('0123456789ABCDEF0'), '0123456789ABCDEF0')
		self.assertEqual(chop_str('0123456789ABCDEF0', 16), '0123456789ABC...')
		self.assertEqual(chop_str('012345678901234567890123456789'), '012345678901234567890...')
		self.assertEqual(chop_str('012345678901234567890123456789', 16), '0123456789012...')
		self.assertEqual(chop_str('012345678901234567890123456789', 32), '012345678901234567890123456789')
		self.assertEqual(chop_str('0123456789012345678901234567890123456', 32), '01234567890123456789012345678...')
		self.assertEqual(chop_str(' '), ' ')
		self.assertEqual(chop_str(' ', 16), ' ')
		self.assertEqual(chop_str(' ', 16, True), '             ...')
		self.assertEqual(chop_str('a'), 'a')
		self.assertEqual(chop_str('a', 16, True), 'a            ...')
		self.assertEqual(chop_str('a', arrange=True), 'a                    ...')
		self.assertEqual(chop_str('a0', 16, True), 'a0           ...')

	def test_is_string_filled_by_character(self) -> None:
		self.assertTrue(is_string_filled_by_character('FFFFFFFFFFFFFFFFFFFFFFFFFFFFF', 'F'))
		self.assertTrue(is_string_filled_by_character('ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ', 'Z'))
		self.assertFalse(is_string_filled_by_character('FAFFFFFFFFFFFFFFFFFFFFFFFFFFF', 'F'))
		self.assertFalse(is_string_filled_by_character('FFFFFFFFFFFFFFFFFFFFFFFFFFFFA', 'F'))
