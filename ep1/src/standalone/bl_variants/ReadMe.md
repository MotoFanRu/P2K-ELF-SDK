bl_variants
===========

This project explores ARM/Thumb Branch Links to provide insights into how ARM GCC and ARM ADS compiles ARM/Thumb code.

## Requirements

* ARM ADS: `tcc`, `armlink`, `fromelf`
* ARM GCC: `arm-none-eabi-gcc`, `arm-none-eabi-objdump`, `arm-none-eabi-objcopy`

## Build

```bash
# Linux
make -f Makefile.eg1
make ARGON=1 -f Makefile.eg1
make -f Makefile.ep1
make ARGON=1 -f Makefile.ep1
```
