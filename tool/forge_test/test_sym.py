# forge_test/test_sym.py

import unittest

from forge import split_and_validate_line


class TestSym(unittest.TestCase):
	def test_split_and_validate_line(self):
		self.assertEqual(split_and_validate_line('0x10C1ACCC T memcpy'), ('0x10C1ACCC', 'T', 'memcpy'))
		self.assertEqual(split_and_validate_line('0x10C1ACCC A memcpy'), ('0x10C1ACCC', 'A', 'memcpy'))
		self.assertEqual(split_and_validate_line('0x10C1ACCC C memcpy'), ('0x10C1ACCC', 'C', 'memcpy'))
		self.assertEqual(split_and_validate_line('0x10C1ACCC D memcpy'), ('0x10C1ACCC', 'D', 'memcpy'))
		self.assertEqual(split_and_validate_line('0x10C1ACCC Z memcpy'), (None, None, None))
		self.assertEqual(split_and_validate_line('0xGGGGGGGG A memcpy'), (None, None, None))
		self.assertEqual(split_and_validate_line('0x10C1ACCC T '), (None, None, None))
		self.assertEqual(split_and_validate_line('A A A'), (None, None, None))
		self.assertEqual(split_and_validate_line(' '), (None, None, None))
