########################################################################################################################
# CMake build script for postlink utility: 26-Nov-2023
#
cmake_minimum_required(VERSION 3.16)

########################################################################################################################
# Project section: postlink
#
project(postlink LANGUAGES ASM C CXX VERSION "1.0.0")

include_directories(${PROJECT_SOURCE_DIR})

#add_definitions("-DLILENDIAN")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -O3")

set(SOURCE_FILES
	constlib.cpp
	libgen.cpp
	postlink.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
