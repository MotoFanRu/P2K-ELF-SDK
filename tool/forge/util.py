# forge/util.py

from datetime import timedelta


def format_timedelta(td: timedelta) -> str:
	days, seconds = td.days, td.seconds
	hours = days * 24 + seconds // 3600
	minutes = (seconds % 3600) // 60
	seconds = (seconds % 60)
	milliseconds = td.microseconds // 1000  # Convert microseconds to milliseconds

	return f'{hours:02d}:{minutes:02d}:{seconds:02d}.{milliseconds:03d}'
