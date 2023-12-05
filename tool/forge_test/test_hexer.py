# forge_test/test_hexer.py

import unittest
from forge import hex2int


class TestHexer(unittest.TestCase):
	def test_hex2int(self):
		self.assertEqual(hex2int('0x00000000'), 0)
		self.assertEqual(hex2int('0x00000001'), 1)
		self.assertEqual(hex2int('0x10080000'), 268959744)
		self.assertEqual(hex2int('0xFFFFFFFF'), 4294967295)

		self.helper_test_hex2_int('0x0')
		self.helper_test_hex2_int('0x1')
		self.helper_test_hex2_int('0x10')
		self.helper_test_hex2_int('0x100')
		self.helper_test_hex2_int('0x1000')
		self.helper_test_hex2_int('0x10000')
		self.helper_test_hex2_int('0x100000')
		self.helper_test_hex2_int('0x1000000')
		# self.helper_test_hex2_int('0x10000000')
		self.helper_test_hex2_int('0')
		self.helper_test_hex2_int('1')
		self.helper_test_hex2_int('0001')
		self.helper_test_hex2_int('1000')
		self.helper_test_hex2_int('G00D')

	def helper_test_hex2_int(self, hex_value):
		with self.assertRaises(ValueError) as context:
			hex2int(hex_value)
