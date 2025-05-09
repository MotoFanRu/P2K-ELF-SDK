# forge_test/test_symbols.py
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

from forge import split_and_validate_line


class TestSymbols(unittest.TestCase):
	def test_split_and_validate_line(self) -> None:
		self.assertEqual(split_and_validate_line('0x10C1ACCC T memcpy'), ('0x10C1ACCC', 'T', 'memcpy'))
		self.assertEqual(split_and_validate_line('0x10C1ACCC A memcpy'), ('0x10C1ACCC', 'A', 'memcpy'))
		self.assertEqual(split_and_validate_line('0x10C1ACCC C memcpy'), ('0x10C1ACCC', 'C', 'memcpy'))
		self.assertEqual(split_and_validate_line('0x10C1ACCC D memcpy'), ('0x10C1ACCC', 'D', 'memcpy'))
		self.assertEqual(split_and_validate_line('0x10C1ACCC Z memcpy'), (None, None, None))
		self.assertEqual(split_and_validate_line('0xGGGGGGGG A memcpy'), (None, None, None))
		self.assertEqual(split_and_validate_line('0x10C1ACCC T '), (None, None, None))
		self.assertEqual(split_and_validate_line('A A A'), (None, None, None))
		self.assertEqual(split_and_validate_line(' '), (None, None, None))
