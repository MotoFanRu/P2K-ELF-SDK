Patterns Utils
==============

Author: **Andy51**

### Building on Linux

```bash
cd P2K-ELF-SDK/tool/patterns
mkdir build
cd build
cmake ..
make VERBOSE=1
strip -s pat
```

### Building on Windows (MinGW)

1. Install [CMake](https://cmake.org/) and add it to the `PATH` environment variable.
2. Install [MinGW + MSYS](https://sourceforge.net/projects/mingw/files/) to the `C:\Dev\MinGW\` directory.
3. Add `C:\Dev\MinGW\bin\` directory to the `PATH` environment variable.
4. Run `cmd.exe` Command Prompt.


```bat
cd P2K-ELF-SDK\tool\patterns
md build
cd build
set PATH=C:\Dev\MinGW\bin;C:\Dev\CMake\bin;%PATH%
cmake -G "MinGW Makefiles" ..
mingw32-make VERBOSE=1
strip -s pat.exe
```

## Additional information

* [Libpat - библиотека для поиска паттерн, инструмент для патчеров](https://forum.motofan.ru/index.php?showtopic=174598)
