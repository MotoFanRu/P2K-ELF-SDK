# forge_test/test_util.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import unittest

from datetime import datetime

from forge import format_timedelta
from forge import chop_string_to_16_symbols


class TestUtil(unittest.TestCase):
	def test_format_timedelta(self):
		now = datetime.now()
		self.assertEqual(len(format_timedelta(datetime.now() - now)), len('00:00:00.000'))

	def test_chop_string_to_16_symbols(self):
		self.assertEqual(chop_string_to_16_symbols('0123456789ABCDEF0'), '0123456789ABC...')
		self.assertEqual(chop_string_to_16_symbols('012345678901234567890123456789'), '0123456789012...')
		self.assertEqual(chop_string_to_16_symbols(' '), '             ...')
		self.assertEqual(chop_string_to_16_symbols('a'), 'a            ...')
		self.assertEqual(chop_string_to_16_symbols('a0'), 'a0           ...')
