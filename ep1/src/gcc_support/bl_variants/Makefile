CC           = arm-none-eabi-gcc
OBJDUMP      = arm-none-eabi-objdump
OBJCOPY      = arm-none-eabi-objcopy
  
OPTIMIZ      = -O2
CPUFLAGS     = -mbig-endian -mthumb -mthumb-interwork -march=armv4t -mtune=arm7tdmi-s
#CPUFLAGS     = -mbig-endian -mthumb -mthumb-interwork -mbe32 -march=armv6j -mtune=arm1136jf-s
FEATURES     = -ffreestanding -fshort-wchar -fshort-enums -fpack-struct=4 -fno-builtin
CFLAGS       = $(CPUFLAGS) $(FEATURES) $(OPTIMIZ) $(INCLUDES)
LDFLAGS      = $(CFLAGS) -nostdlib -T bl.ld -Wl,--gc-sections

# EQU
TARGET1_ELF  = bl_1.elf
# SYM
TARGET2_ELF  = bl_2.elf
# ASM LIB
TARGET3_ELF  = bl_3.elf
# DEFINES
TARGET4_ELF  = bl_4.elf

all:
	$(CC) $(CFLAGS) -c lib.S -o lib.o
	$(CC) $(CFLAGS) -c lib_equ.S -o lib_equ.o
	$(CC) $(CFLAGS) -c bl.c -o bl.o
	$(CC) -DDEFINES $(CFLAGS) -c bl.c -o bld.o
	$(CC) $(LDFLAGS) bl.o lib_equ.o -o $(TARGET1_ELF)
	$(CC) $(LDFLAGS) -Wl,-R lib.sym bl.o -o $(TARGET2_ELF)
	$(CC) $(LDFLAGS) bl.o lib.o -o $(TARGET3_ELF)
	$(CC) $(LDFLAGS) bld.o -o $(TARGET4_ELF)
	$(OBJCOPY) -O binary -j .text* $(TARGET1_ELF) $(TARGET1_ELF).bin
	$(OBJCOPY) -O binary -j .text* $(TARGET2_ELF) $(TARGET2_ELF).bin
	$(OBJCOPY) -O binary -j .text* $(TARGET3_ELF) $(TARGET3_ELF).bin
	$(OBJCOPY) -O binary -j .text* $(TARGET4_ELF) $(TARGET4_ELF).bin
	$(OBJDUMP) --disassemble=UtilLogStringData $(TARGET1_ELF)
	$(OBJDUMP) --disassemble=UtilLogStringData $(TARGET2_ELF)
	$(OBJDUMP) --disassemble=UtilLogStringData $(TARGET3_ELF)
	$(OBJDUMP) --disassemble=UtilLogStringData $(TARGET4_ELF)
	ls -alh *.bin
	@for file in *.bin; do echo "===== $$file ====="; hexdump -C "$$file"; done

clean:
	-rm -f *.o
	-rm -f *.elf
	-rm -f *.bin
