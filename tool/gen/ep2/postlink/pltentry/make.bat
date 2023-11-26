:: General build script

if /I "%1"=="clean" (
	if exist pltentry.o del pltentry.o
	exit /b 0
)

set P2K_ELF_SDK_PATH=..\..\..\..\..
set COMPILER_WINDOWS_EP1_PATH=%P2K_ELF_SDK_PATH%\tool\win\ep1\ADS12_848_Windows

%COMPILER_WINDOWS_EP1_PATH%\Bin\armasm -16 -apcs /interwork pltentry.asm

rem -bigend
rem armlink -ro-base 0x10D00000 -o dummy.elf dummy.o
rem fromelf dummy.elf -bin -output dummy.bin
rem pause
