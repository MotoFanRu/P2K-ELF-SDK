armasm.exe -16 -apcs /interwork pltentry.asm
rem -bigend
rem armlink -ro-base 0x10D00000 -o dummy.elf dummy.o
rem fromelf dummy.elf -bin -output dummy.bin
rem pause