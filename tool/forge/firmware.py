# forge/firmware.py

def parse_phone_firmware(phone_firmware_filename: str) -> list[str]:
	phone_firmware = []
	segments_a = phone_firmware_filename.split('_')  # First split string by '_'.
	segments_b = phone_firmware_filename.split('.')  # Second split string by '.'.
	segments_a_ok = len(segments_a) > 2              # At least 3 segments in segments_a.
	segments_b_ok = len(segments_b) > 2              # At least 3 segments in segments_b.
	extension = segments_b[-1]                       # Please use *.smg or *.bin extensions.
	extension_ok = (extension == 'smg') or (extension == 'bin')

	if not (segments_a_ok and segments_b_ok and extension_ok):
		raise ValueError('Wrong phone-firmware name format! Please use this pattern: E1_R373_G_0E.30.49R.smg')

	phone_name = segments_a[0]  # First segment of first split will be a phone name.
	firmware_name = phone_firmware_filename.replace(f'{phone_name}_', '').replace(f'.{extension}', '')

	phone_firmware.append(phone_name)
	phone_firmware.append(firmware_name)

	return phone_firmware
