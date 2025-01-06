Hello Moto
==========

A simple "Hello, World!" program for Motorola P2K phones with EP1 ElfLoader.

## Requirements

* ARM ADS: `tcc`, `armlink`, `fromelf`
* ARM GCC: `arm-none-eabi-gcc`, `arm-none-eabi-objdump`, `arm-none-eabi-objcopy`

## Build

```bash
make -f Makefile.eg1          # Linux, EG1
make ARGON=1 -f Makefile.eg1  # Linux, EG1, EA1
make -f Makefile.ep1          # Linux, EP1
make ARGON=1 -f Makefile.ep1  # Linux, EP1, EA1
```
