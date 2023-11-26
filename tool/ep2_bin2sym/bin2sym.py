#!/usr/bin/env python

"""
bin2sym.py script by EXL, 22-Nov-2023

Generate symdefs map file from binary libraries.

One-line helpers:
	cat lib1.sym | sort -u > 1.sym

	grep -v '^#' lib2.sym | awk -Wnon-decimal-data '{if ($1>0) printf "0x%08X %s %s\n", 0+$1, $2, $3}' | sort -u > 2.sym

	git diff 1.sym 2.sym
"""

import os
import sys
import re

from ctypes import *

class NameEntry:
	def __init__(self, type, name, address):
		self.type = type
		self.name = name.strip()
		self.address = int(address, 16)

class LibraryEntry:
	def __init__(self, index, address, type):
		self.name = 'WARNING_WARNING_WARNING_UNKNOWN_FUNCTION_NAME'
		self.index = index
		if type == 'Function':
			if address % 2 == 0:
				self.type = 'A'
				self.address = address
			else:
				self.type = 'T'
				self.address = address - 1
		elif type == 'Constant':
			self.type = 'C'
			self.address = address

	def sym(self):
		if self.name.startswith('WARNING_'):
			ent_name = self.name + '_' + f'0x{self.index:08X}'
		else:
			ent_name = self.name
		sym = f'0x{self.address:08X} {self.type} {ent_name}'
		print(sym)
		return sym + '\n'

class ConstEntry:
	def __init__(self, index):
		self.name = 'WARNING_WARNING_WARNING_UNKNOWN_CONSTANT_NAME'
		self.index = index
		self.value = 0xFFFFFFFF

class header(BigEndianStructure):
	_fields_ = [
		('magic', c_uint),
		('version', c_char * 8),
		('firmware', c_char * 24),
		('symCnt', c_uint),
		('strTabSz', c_uint),
		('strTabOff', c_uint),
		('constCnt', c_uint),
		('constOff', c_uint)
	]

def generate_symdefs_file(symdefs, library):
	pass

def read_library_header(library):
	hdr = header()
	library.readinto(hdr)

	print('magic=' + hex(hdr.magic))
	print('version=' + hdr.version.decode())
	print('firmware=' + hdr.firmware.decode())
	print('symCnt=' + str(hdr.symCnt))
	print('strTabSz=' + str(hdr.strTabSz))
	print('strTabOf=' + str(hdr.strTabOff))
	print('constCnt=' + str(hdr.constCnt))
	print('constOff=' + str(hdr.constOff))
	print()

	if hdr.magic != 0x7F4C4942:
		raise Exception('Wrong Magic Header!')

	return hdr

def convert_library_bin_to_library_sym(library_file, names_file):
	def_names = []
	library_model = []
	const_model = []
	const_indexes = []
	const_values = []

	# => Names.
	try:
		with open(names_file, 'r') as names:
			for line in names:
				line = line.strip()
				if line:
					(type, name, address) = line.split()
					def_names.append(NameEntry(type, name, address))
	except Exception as error:
		print('Warning! Names content is empty!')
		print(error)

	# => Library.
	try:
		with open(library_file, 'rb') as library:
			hdr = read_library_header(library)

			# Indexes and addresses.
			for i in range(sizeof(hdr), hdr.strTabOff, sizeof(c_uint) * 2):
				library_model.append(
					LibraryEntry(
						int.from_bytes(library.read(4), 'big'),
						int.from_bytes(library.read(4), 'big'),
						'Function'
					)
				)
			print('library_model_len=' + str(len(library_model)))

			# Function names.
			fun_name = ''
			cnt = 0
			for i in range(hdr.strTabOff, hdr.constOff, 1):
				ch = library.read(1)
				if ch != b'\x00':
					fun_name += ch.decode()
				else:
					library_model[cnt].name = fun_name.strip()
					cnt += 1
					fun_name = ''
			print('function_names_len=' + str(cnt))
			print()

			# Constants.
			for i in range(0, hdr.constCnt, 1):
				const_model.append(ConstEntry(int.from_bytes(library.read(2), 'big')))
			for i in range(0, hdr.constCnt, 1):
				const_model[i].value = int.from_bytes(library.read(4), 'big')
			print('const_model_len=' + str(len(const_model)))
			print()
	except Exception as error:
		print('Error! Cannot open library file!')
		print(error)
		return

	# Resolve names.
	for lib_entry in library_model:
		for name_entry in def_names:
			if lib_entry.name == name_entry.name:
				if name_entry.type == 'D':
					print('D => ' + name_entry.name)
					lib_entry.type = 'D'
	print()
	for const_entry in const_model:
		for name_entry in def_names:
			if const_entry.index == name_entry.address:
				if name_entry.type == 'C':
					print('С => ' + hex(name_entry.address) + ' => ' + name_entry.name)
					const_entry.name = name_entry.name.strip()
	print()

	# Sorting.
	library_model.sort(key = lambda o: o.name)
	const_model.sort(key = lambda o: o.index)

	# Insert consts to library.
	for const_entry in const_model:
		library_entry = LibraryEntry(const_entry.index, const_entry.value, 'Constant')
		library_entry.name = const_entry.name
		library_model.append(library_entry)

	# Generate symdefs file.
	try:
		with open(library_file.replace('.bin', '.sym'), 'w') as symdefs:
			for library_entry in library_model:
				symdefs.write(library_entry.sym())
	except Exception as error:
		print('Error! Cannot create/write symdefs file!')
		print(error)

if __name__ == '__main__':
	if len(sys.argv) == 2:
		convert_library_bin_to_library_sym(sys.argv[1], None)
	elif len(sys.argv) == 3:
		convert_library_bin_to_library_sym(sys.argv[2], sys.argv[1])
	else:
		print('bin2sym.py script by EXL, 22-Nov-2023')
		print('Generate symdefs map file from binary libraries.')
		print()
		print('Usage:\n\t./bin2sym.py [names] [library]')
		print('Example:\n\t./bin2sym.py names.def library.bin')
