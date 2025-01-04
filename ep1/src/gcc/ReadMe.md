ElfPack v1.x Standalone
=======================

This is an example of how to build an ElfPack patch using ARM ADS and ARM GCC compilers outside of a **[kitchen](../../../tool/kitchen/)** environment.

## Requirements

* ARM ADS: `tcc`, `armlink`, `fromelf`
* ARM GCC: `arm-none-eabi-gcc`, `arm-none-eabi-objdump`, `arm-none-eabi-objcopy`

## Build

```bash
# Linux
make -f Makefile.eg1
make ARGON=1 -f Makefile.eg1
```
