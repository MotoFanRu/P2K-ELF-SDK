cmake_minimum_required(VERSION 3.16)

if(NOT ELFPACK)
	set(ELFPACK "EP1")
endif()

if(NOT CPU_MODE)
	set(CPU_MODE "thumb")
endif()

set(P2K_TOOLCHAIN_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

if(ELFPACK STREQUAL "EP1") # ElfPack v1.x, ARM/Thumb.
	include("${P2K_TOOLCHAIN_ROOT}/cmake/P2K_EP1_Toolchain.cmake")
elseif(ELFPACK STREQUAL "EP2") # ElfPack v2.x, ARM/Thumb.
	include("${P2K_TOOLCHAIN_ROOT}/cmake/P2K_EP2_Toolchain.cmake")
elseif(ELFPACK STREQUAL "EM1") # ElfPack v1.x, M*CORE.
	include("${P2K_TOOLCHAIN_ROOT}/cmake/P2K_EM1_Toolchain.cmake")
elseif(ELFPACK STREQUAL "EM2") # ElfPack v2.x, M*CORE.
	include("${P2K_TOOLCHAIN_ROOT}/cmake/P2K_EM2_Toolchain.cmake")
endif()
