########################################################################################################################
# CMake build script for Motorola P2K ELFs: 26-Nov-2023
#
cmake_minimum_required(VERSION 3.16)

set(ELFPACK "EP1" CACHE STRING "Default value for ELFPACK")
set(CPU_MODE "thumb" CACHE STRING "Default value for CPU_MODE")
include("../../../../cmake/P2K_ELF_SDK.cmake")

########################################################################################################################
# Project section: ini
#
project(ini LANGUAGES CXX VERSION "1.0.0")

include_directories(${PROJECT_SOURCE_DIR})

add_definitions("-DOPTIMIZE_1")

set(SOURCE_FILES
	ccfile.cpp
	cini.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
