ElfPack v1.x ARM GCC
====================

This is an example of how to build an ElfPack v1.x patch using ARM GCC compiler outside of a kitchen environment.

## Requirements

* `arm-none-eabi-gcc`, `arm-none-eabi-objdump`, `arm-none-eabi-objcopy`

## Build and Run

```bash
# Linux
make -f Makefile.eg1
make ARGON=1 -f Makefile.eg1
```
