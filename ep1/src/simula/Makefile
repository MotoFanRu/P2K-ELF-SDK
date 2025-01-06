# Script: General Linux Makefile
# Author: EXL, 05-Jan-2025

P2K_ELF_SDK       = ../../..

CC                = gcc

INCLUDES          = -I../ -I$(P2K_ELF_SDK)/sdk  -I$(P2K_ELF_SDK)/ep1/inc
DEFINES           = -D__P2K__ -DSIMULA
WARNINGS          = -Wall -Wextra -Wno-int-conversion -Wno-format -Wno-unused -Wno-unused-result
OPTIMIZATIONS     = -O2
FEATURES          = -fshort-wchar -fshort-enums -fpack-struct=4
CPU_FLAGS         = -m32
ifeq ($(ARGON), 1)
	FLAVOR        = -DEA1
else
	FLAVOR        = -DEG1
endif
CFLAGS            = $(INCLUDES) $(DEFINES) $(FLAVOR) $(WARNINGS) $(OPTIMIZATIONS) $(CPU_FLAGS) $(FEATURES)
CXXFLAGS          = $(CFLAGS)
LDFLAGS           = $(CFLAGS) -s -Wl,--gc-sections
LIBS              =

TARGET            = Simula

all:
	$(CC) $(CFLAGS) -c Simula.c -o Simula.o
	$(CC) $(LDFLAGS) Simula.o $(LIBS) -o $(TARGET)

clean:
	-rm -f *.o
	-rm -f *.bin
	-rm -f $(TARGET)
