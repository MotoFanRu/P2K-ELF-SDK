########################################################################################################################
# CMake build script for pat utility: 05-Dec-2023
#
cmake_minimum_required(VERSION 3.16)

########################################################################################################################
# Project section: pat
#
project(pattern LANGUAGES C CXX VERSION "1.0.0")

if(CMAKE_HOST_WIN32)
	set(LIB_FIND_FUNC "libfind_func_win_x86.a")
elseif(CMAKE_HOST_UNIX)
	set(LIB_FIND_FUNC "libfind_func_lin_x86.a")
endif()

include_directories(${PROJECT_SOURCE_DIR})

#add_definitions("-DLIBPAT_EXPORTS")

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -m32 -O2 -no-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -O2 -no-pie")
if(CMAKE_HOST_WIN32)
	set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -static")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
endif()

set(SOURCE_FILES
	api.c
	libpat.cpp
	parser.cpp
	ramtrans.cpp)

add_executable(pat pat.cpp ${SOURCE_FILES})
target_link_libraries(pat "${CMAKE_SOURCE_DIR}/find_func/${LIB_FIND_FUNC}")
