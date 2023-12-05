# forge/hexer.py

def hex2int(hex_value: str) -> int:
	if not hex_value.startswith('0x'):
		raise ValueError(f'value "{hex_value}" should starts with a "0x" prefix')
	if len(hex_value) != (8 + 2):  # 0x12345678
		raise ValueError(f'value "{hex_value}" should be in the "8 + 2" format like "0x12345678" hex digit')
	try:
		return int(hex_value, 16)
	except ValueError:
		raise ValueError(f'value "{hex_value}" is not a valid hexadecimal value')
