Find Patterns SSE Optimized Library
===================================

Author: **Andy51**

## Building

Download `uasm` or `jwasm` MASM compatible assembler.

1. [UASM](https://www.terraspace.co.uk/uasm.html)
2. [JWasm](https://github.com/JWasm/JWasm)


### Linux

```bash
./uasm -zcw -zze -zt0 -zv0 -coff findPattern.asm
./uasm -zcw -zze -zt0 -zv0 -coff findAllPatterns.asm

./jwasm -zcw -zze -zt0 -zv0 -coff findPattern.asm
./jwasm -zcw -zze -zt0 -zv0 -coff findAllPatterns.asm

ar rcs libfind_func_lin_x86.a findPattern.o findAllPatterns.o
```

### Windows (MinGW)

```bash
./uasm32 -coff findPattern.asm
./uasm32 -coff findAllPatterns.asm

ar rcs libfind_func_win_x86.a findPattern.obj findAllPatterns.obj
```

## Additional information

* [Libpat - библиотека для поиска паттерн, инструмент для патчеров](https://forum.motofan.ru/index.php?showtopic=174598)
