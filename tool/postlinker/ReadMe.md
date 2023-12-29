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

1. Install [CMake](https://cmake.org/) and add it to the `PATH` environment variable.
2. Install [MinGW + MSYS](https://sourceforge.net/projects/mingw/files/) to the `C:\Dev\MinGW\` directory.
3. Add `C:\Dev\MinGW\bin\` directory to the `PATH` environment variable.
4. Run `cmd.exe` Command Prompt.


```bat
set PATH=C:\Dev\MinGW\bin;C:\Dev\CMake\bin;%PATH%
cmake -G "MinGW Makefiles" ..
mingw32-make VERBOSE=1
strip -s postlink.exe
```

## Additional information

* [ElfPack v2.0, Начало всеобщего тестирования](https://forum.motofan.ru/index.php?showtopic=161718)
