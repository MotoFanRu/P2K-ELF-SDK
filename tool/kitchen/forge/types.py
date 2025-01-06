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

import configparser

from enum import Enum
from typing import TypeAlias


PatchDict: TypeAlias = dict[str, str]
PatchDictNone: TypeAlias = dict[str, str] | None
Symbol: TypeAlias = tuple[str | None, str | None, str | None]
LibraryModel: TypeAlias = list[tuple[str, str, str]]
NamesDefs: TypeAlias = dict[str, str]
PatternModel: TypeAlias = list[tuple[str, str, str, str]]


class ElfPack(Enum):
	EP1: int = 0  # ElfPack v1.x, ARM ADS, Neptune, ARMv4T / ARM7TDMI-S.
	EP2: int = 1  # ElfPack v2.x, ARM GCC, Neptune, ARMv4T / ARM7TDMI-S.
	EM1: int = 2  # ElfPack v1.x, M*CORE GCC, Rainbow POG, M-CORE M341S.
	EM2: int = 3  # ElfPack v2.x, M*CORE GCC, Rainbow POG, M-CORE M341S.
	EG1: int = 4  # ElfPack v1.x, ARM GCC, Neptune, ARMv4T / ARM7TDMI-S.
	EA1: int = 5  # ElfPack v1.x, ARM ADS or ARM GCC, ArgonLV, ARMv6J / ARM1136JF-S.
	UNK: int = 6  # Unknown ElfPack version.


ElfPacks: TypeAlias = tuple[ElfPack, ElfPack]


class LibrarySort(Enum):
	ADDR: int = 0
	MODE: int = 1
	NAME: int = 2
	NONE: int = 3


# Case-sensitive config parser.
class CsConfigParser(configparser.ConfigParser):
	def optionxform(self, option: any) -> any:
		return option


class MemoryRegion(Enum):
	IROM: int = 0
	IRAM: int = 1
	ROM: int = 2
	RAM: int = 3
	PERIPHERALS: int = 4
	UNKNOWN: int = 5
