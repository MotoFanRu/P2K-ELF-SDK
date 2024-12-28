# Just an example how to build ElfLoader 1 using GCC.

CC         = arm-none-eabi-gcc
OBJCOPY    = arm-none-eabi-objcopy
OBJDUMP    = arm-none-eabi-objdump

OPTIMIZ    = -O2
DEFINES    = -D__P2K__ -DEP1
WARNINGS   = -Wall
CPUFLAGS   = -mbig-endian -march=armv4t -mtune=arm7tdmi-s -mthumb -mthumb-interwork
FEATURES   = -ffreestanding -fshort-wchar -fshort-enums -fpack-struct=4 -fno-builtin
INCLUDES   = -I../../../sdk
CFLAGS     =  $(OPTIMIZ) $(WARNINGS) $(CPUFLAGS) $(FEATURES) $(DEFINES) $(INCLUDES)
LDFLAGS    = $(CFLAGS) -nostdlib -T ElfPack.ld -Wl,--gc-sections

TARGET     = ElfPack
TARGET_ELF = $(TARGET).elf
TARGET_BIN = $(TARGET).bin

all:
	sed 's/%addr_entry%/0x10901700/g' ../../tpl/ElfPack.tpl > ElfPack.ld
	$(CC) $(CFLAGS) -c Lib.S -o Lib.o
	$(CC) $(CFLAGS) -c SysInfo.c -o SysInfo.o
	$(CC) $(CFLAGS) -c ../AFW_CreateInternalQueuedEv_Wrappers.c -o AFW_CreateInternalQueuedEv_Wrappers.o
	$(CC) $(CFLAGS) -c ../AutoRun.c -o AutoRun.o
	$(CC) $(CFLAGS) -c ../ElfLoader.c -o ElfLoader.o
	$(CC) $(CFLAGS) -c ../ElfLoaderApp.c -o ElfLoaderApp.o
	$(CC) $(LDFLAGS) AutoRun.o ElfLoaderApp.o ElfLoader.o AFW_CreateInternalQueuedEv_Wrappers.o SysInfo.o Lib.o \
		-o $(TARGET_ELF)
	$(OBJCOPY) -O binary -j .text* $(TARGET_ELF) $(TARGET_BIN)
	$(OBJDUMP) -x $(TARGET_ELF) | grep -E 'AutorunMain|Ldr|Util|u_u|namecmp'
	md5sum $(TARGET_ELF) $(TARGET_BIN)

clean:
	-rm -f *.ld
	-rm -f *.o
	-rm -f *.elf
	-rm -f *.bin
