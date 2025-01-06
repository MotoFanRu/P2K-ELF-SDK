Simula
======

The **Simula** project is designed to debug the ElfLoader part of ElfPack, which is used to launch ELFs.

## Requirements

* GCC

## Build

```bash
make          # Linux, EG1
make ARGON=1  # Linux, EG1, EA1
```

## Run & Test

The `-r` flag is used to enable relocations.

```bash
./simula TestData/elfloader.lib TestData/HelloMoto_ADS.elf
./simula TestData/elfloader.lib TestData/HelloMoto_ADS.elf -r
vbindiff TestData/ELF_MEMORY_DUMP_ADS.bin ELF_MEMORY_DUMP.bin

./simula TestData/elfloader.lib TestData/HelloMoto_GCC.elf
./simula TestData/elfloader.lib TestData/HelloMoto_GCC.elf -r
vbindiff TestData/ELF_MEMORY_DUMP_GCC.bin ELF_MEMORY_DUMP.bin
```
