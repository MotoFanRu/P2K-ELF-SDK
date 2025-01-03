clang-format configuration
==========================

Use this configuration file to format all source code merged into this repository.

## Requirements

* `clang-format` utility version 15.x.x and above.

## Usage

```bash
clang-format -i --style=file:../../../tool/clang-format/clang-format.conf *.h *.hpp *.c *.cpp *.cxx *.C *.H
```
