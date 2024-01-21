PostLinker
==========

Postlinker utility for finalizing ElfPack v2.0 artifacts.

Author: **Andy51**

### Building on Linux

```bash
cmake ..
make VERBOSE=1
strip -s postlink
```

### Building on Windows (MinGW)

1. Install [CMake](https://cmake.org/) to the `C:\Dev\MinGW\` directory and add `C:\Dev\MinGW\bin` to the `PATH` environment variable.
2. Install [MinGW + MSYS](https://sourceforge.net/projects/mingw/files/) to the `C:\Dev\CMake\` directory and add `C:\Dev\CMake\bin` to the `PATH` environment variable.
3. Install `mingw-developer-toolkit`, `mingw32-base`, `mingw32-gcc-g++`, `msys-base` packages in the MinGW Installation Manager.
4. Run `cmd.exe` Command Prompt in current directory and enter commands.

```bat
set PATH=C:\Dev\MinGW\bin;C:\Dev\CMake\bin;%PATH%
cmake -G "MinGW Makefiles" ..
mingw32-make VERBOSE=1
strip -s postlink.exe
```

## Additional information

* [ElfPack v2.0, Начало всеобщего тестирования](https://forum.motofan.ru/index.php?showtopic=161718)
