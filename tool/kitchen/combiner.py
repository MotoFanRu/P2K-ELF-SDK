#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
An utility for combining various text files line by line. Useful for combining symbols (*.sym) files.

Python: 3.10+
License: MIT
Authors: EXL, MotoFan.Ru, GitHub Copilot (ChatGPT 4o)
Date: 27-Feb-2025
Version: 1.0

Usage:
	./combiner.py -o C390.sym -i C390_8.sym C390_16.sym C390_32.sym
"""

import argparse


def read_files(file_paths):
	file_contents = []
	for file_path in file_paths:
		with open(file_path, 'r') as f:
			file_contents.append(f.readlines())
	return file_contents


def combine_files(file_contents):
	combined_lines = []
	max_lines = max(len(contents) for contents in file_contents)

	for i in range(max_lines):
		lines_at_i = [contents[i].strip() if i < len(contents) else '' for contents in file_contents]
		if all(line == lines_at_i[0] for line in lines_at_i):
			combined_lines.append(lines_at_i[0])
		else:
			addresses = [line for line in lines_at_i if not line.startswith('# NOT_FOUND:')]
			if addresses:
				if all(addr.split()[0] == addresses[0].split()[0] for addr in addresses):
					print(f'OK   : {addresses}')
				else:
					print(f'FAIL : {addresses}')
				combined_lines.append(addresses[0])

	return combined_lines


def write_output(output_path, combined_lines):
	with open(output_path, 'w') as f:
		for line in combined_lines:
			f.write(line + '\n')


def main():
	parser = argparse.ArgumentParser(description='Concatenate text files line by line.')
	parser.add_argument('-o', '--output', required=True, help='Output file path')
	parser.add_argument('-i', '--input', required=True, nargs='+', help='Input file paths')

	args = parser.parse_args()

	file_contents = read_files(args.input)
	combined_lines = combine_files(file_contents)
	write_output(args.output, combined_lines)

if __name__ == '__main__':
	main()
