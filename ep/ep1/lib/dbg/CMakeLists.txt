########################################################################################################################
# CMake build script for Motorola P2K ELFs: 26-Nov-2023
#
cmake_minimum_required(VERSION 3.16)

if(NOT ELFPACK)
	set(ELFPACK "EP1")
endif()
if(NOT CPU_MODE)
	set(CPU_MODE "thumb")
endif()

set(CMAKE_TOOLCHAIN_FILE "../../../../cmake/P2K_ELF_SDK.cmake")

########################################################################################################################
# Project section: dbg
#
project(dbg LANGUAGES C VERSION "1.0.0")

include_directories(${PROJECT_SOURCE_DIR})

set(SOURCE_FILES
	dbg.c)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
