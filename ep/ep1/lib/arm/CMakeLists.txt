########################################################################################################################
# CMake build script for Motorola P2K ELFs: 26-Nov-2023
#
cmake_minimum_required(VERSION 3.16)

set(ELFPACK "EP1" CACHE STRING "Default value for ELFPACK" FORCE)
set(CPU_MODE "thumb" CACHE STRING "Default value for CPU_MODE" FORCE)

set(CMAKE_TOOLCHAIN_FILE "../../../../cmake/P2K_ELF_SDK.cmake")

########################################################################################################################
# Project section: arm
#
project(arm LANGUAGES ASM VERSION "1.0.0")

include_directories(${PROJECT_SOURCE_DIR})

set(SOURCE_FILES
	arm.S)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
