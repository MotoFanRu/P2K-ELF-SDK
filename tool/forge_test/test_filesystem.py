# forge_test/test_filesystem.py
# -*- coding: utf-8 -*-

"""
A special "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
"""

import unittest

from forge import normalize_extension


class TestFileSystem(unittest.TestCase):
	def test_normalize_extension(self) -> None:
		self.assertEqual(normalize_extension(' '), '. ')  # ???
		self.assertEqual(normalize_extension('ext'), '.ext')
		self.assertEqual(normalize_extension('.ext'), '.ext')
