# forge/types.py
# -*- coding: utf-8 -*-

"""
The "Forge" python library for the P2K ELF SDK toolchain.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru
Date: 15-Dec-2023
Version: 1.0
"""

from typing import TypeAlias


PatchDict: TypeAlias = dict[str, str]
PatchDictNone: TypeAlias = dict[str, str] | None
Symbol: TypeAlias = tuple[str | None, str | None, str | None]
LibraryModel: TypeAlias = list[tuple[str, str, str]]
NamesDefs: TypeAlias = dict[str, str]
