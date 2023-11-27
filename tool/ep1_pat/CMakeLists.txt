########################################################################################################################
# CMake build script for pat utility: 27-Nov-2023
#
cmake_minimum_required(VERSION 3.16)

########################################################################################################################
# Project section: pat
#
project(pat LANGUAGES ASM C CXX VERSION "1.0.0")

if(CMAKE_HOST_WIN32)
	set(LIB_FIND_PATTERNS "libfind_patterns_win_x86.a")
elseif(CMAKE_HOST_UNIX)
	set(LIB_FIND_PATTERNS "libfind_patterns_lin_x86.a")
endif()

include_directories(${PROJECT_SOURCE_DIR})

#add_definitions("-DLIBPAT_EXPORTS")

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -m32 -O2 -no-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -O2 -no-pie")

set(SOURCE_FILES
	api.c
	libpat.cpp
	parser.cpp
	ramtrans.cpp
	pat.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/find_patterns/${LIB_FIND_PATTERNS}")
