#!/usr/bin/env python3

import sys

r373_firmware = [
	("CG1.bin", 0x80000, 0xD00000),
	("CG3.bin", 0x40000, 0x80000),
	("CG7.bin", 0x1F80000, 0x1FE0000),
	("CG18.bin", 0x1FE0000, 0x2000000),
]

def trim_and_pad_file(filename):
	try:
		with open(filename, 'rb') as file:
			data = file.read()
		if all(byte == 0xFF for byte in data) or not data:
			last_useful_byte_index = -1
		else:
			last_useful_byte_index = max(i for i, byte in enumerate(data) if byte != 0xFF)
		trimmed_data = data[:last_useful_byte_index + 1]
		pad_length = (-len(trimmed_data)) % 0x10
		padded_data = trimmed_data + (b'\xFF' * pad_length)
		with open(filename, 'wb') as file:
			file.write(padded_data)
		print(f"File '{filename}' has been trimmed, padded, and saved.")
	except FileNotFoundError:
		print("The specified file does not exist.")
	except Exception as e:
		print(f"An error occurred: {e}")

def cut_binary_file(input_filename, output_filename, start_address, end_address):
	try:
		with open(input_filename, 'rb') as original_file:
			original_file.seek(start_address)
			size_to_read = end_address - start_address
			data = original_file.read(size_to_read)
			with open(output_filename, 'wb') as new_file:
				new_file.write(data)
		print(f"File {output_filename} has been cut from 0x{start_address:08X} to 0x{end_address:08X} and saved as {output_filename}")
	except FileNotFoundError:
		print("The specified input file does not exist.")
	except Exception as e:
		print(f"An error occurred: {e}")

def main():
	if len(sys.argv) != 2:
		print("Usage: python3 firmware_cutter.py <input_binary_file>")
		print("\tpython3 firmware_cutter.py firmware.bin")
		sys.exit(1)

	for filename, start, end in r373_firmware:
		cut_binary_file(sys.argv[1], filename, start, end)
		trim_and_pad_file(filename)

if __name__ == "__main__":
	main()
