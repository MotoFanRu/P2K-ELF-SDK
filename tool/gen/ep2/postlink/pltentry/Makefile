# General build script

P2K_ELF_SDK_PATH=../../../../..
COMPILER_LINUX_EP1_PATH=$(P2K_ELF_SDK_PATH)/tool/lin/ep1/ADS12_848_Linux

all:
	$(COMPILER_LINUX_EP1_PATH)/bin/armasm -16 -apcs /interwork pltentry.asm

clean:
	-rm pltentry.o
