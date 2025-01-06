ElfPack v1.x Source Code
========================

Improved original ElfPack v1.x source code released by **Andy51** on 23-Jan-2024, featuring fixes and added functionality, including loading and executing ELFs built with modern ARM GCC.

## Subprojects

* [goldsrc](goldsrc):

  Original ElfPack source code released by **Andy51** on 23-Jan-2024, corrected for use with modern SDK. ABI and API compatibility and equivalence are preserved.

* [simula](simula):

  The **Simula** project is designed to debug the ElfLoader part of ElfPack, which is used to launch ELFs.

* [standalone](standalone):

  This is an example of how to build an ElfPack patch using ARM ADS and ARM GCC compilers outside of a **[kitchen](../../tool/kitchen/)** environment.
