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

from enum import Enum
from typing import TypeAlias


PatchDict: TypeAlias = dict[str, str]
PatchDictNone: TypeAlias = dict[str, str] | None
Symbol: TypeAlias = tuple[str | None, str | None, str | None]
LibraryModel: TypeAlias = list[tuple[str, str, str]]
NamesDefs: TypeAlias = dict[str, str]


class ElfPack(Enum):
	EP1: int = 0  # ElfPack v1.0, ARM.
	EP2: int = 1  # ElfPack v2.0, ARM.
	EM1: int = 2  # ElfPack v1.0, M*CORE.
	EM2: int = 3  # ElfPack v2.0, M*CORE.
	UNK: int = 4  # Unknown ElfPack version.


ElfPacks: TypeAlias = tuple[ElfPack, ElfPack]
