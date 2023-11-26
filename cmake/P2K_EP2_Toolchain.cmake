cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_PROCESSOR "ARM")

if(CMAKE_HOST_WIN32)
	set(TOOLCHAIN_DIRECTORY "tool/ep2_win_devkitARM")
elseif(CMAKE_HOST_UNIX)
	set(TOOLCHAIN_DIRECTORY "tool/ep2_lin_devkitARM")
else()
	message(FATAL_ERROR "Unknown OS!")
endif()

set(CMAKE_C_COMPILER "${P2K_TOOLCHAIN_ROOT}/${TOOLCHAIN_DIRECTORY}/bin/arm-eabi-gcc")
set(CMAKE_CXX_COMPILER "${P2K_TOOLCHAIN_ROOT}/${TOOLCHAIN_DIRECTORY}/bin/arm-eabi-g++")
#set(CMAKE_ASM_COMPILER "${P2K_TOOLCHAIN_ROOT}/${TOOLCHAIN_DIRECTORY}/bin/arm-eabi-as")
set(CMAKE_ASM_COMPILER "${P2K_TOOLCHAIN_ROOT}/${TOOLCHAIN_DIRECTORY}/bin/arm-eabi-gcc")
set(CMAKE_AR "${P2K_TOOLCHAIN_ROOT}/${TOOLCHAIN_DIRECTORY}/bin/arm-eabi-ar")
set(CMAKE_C_LINK_EXECUTABLE "${P2K_TOOLCHAIN_ROOT}/${TOOLCHAIN_DIRECTORY}/bin/arm-eabi-ld")
set(CMAKE_CXX_LINK_EXECUTABLE "${P2K_TOOLCHAIN_ROOT}/${TOOLCHAIN_DIRECTORY}/bin/arm-eabi-ld")
set(CMAKE_OBJCOPY "${P2K_TOOLCHAIN_ROOT}/${TOOLCHAIN_DIRECTORY}/bin/arm-eabi-objcopy")
set(CMAKE_POSTLINK "${P2K_TOOLCHAIN_ROOT}/tool/postlink")
if(CMAKE_HOST_WIN32)
	set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER}.exe")
	set(CMAKE_CXX_COMPILER "${CMAKE_CXX_COMPILER}.exe")
	set(CMAKE_ASM_COMPILER "${CMAKE_ASM_COMPILER}.exe")
	set(CMAKE_AR "${CMAKE_AR}.exe")
	set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE}.exe")
	set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}.exe")
	set(CMAKE_OBJCOPY "${CMAKE_OBJCOPY}.exe")
	set(CMAKE_POSTLINK "${CMAKE_POSTLINK}.exe")
endif()

set(CMAKE_C_COMPILER_ID "GNU")
set(CMAKE_C_COMPILER_ID_RUN TRUE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_ID "GNU")
set(CMAKE_CXX_COMPILER_ID_RUN TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

set(P2K_DIRECTORY_HEADERS_SDK "${P2K_TOOLCHAIN_ROOT}/ep/sdk")
set(P2K_DIRECTORY_HEADERS_LIBRARIES "${P2K_TOOLCHAIN_ROOT}/ep/ep2/inc")
set(P2K_DIRECTORY_LIBRARIES "${P2K_TOOLCHAIN_ROOT}/ep/ep2/lib")

set(P2K_GENERAL_DEFINES_FLAGS "-D__P2K__ -DEP2")
set(P2K_GENERAL_OPTIMIZATION_FLAGS "-O2")
set(P2K_LIB_STD "std.sa")
set(P2K_LIB_C "libc.a")

if(CPU_MODE STREQUAL "thumb")
	set(P2K_GENERAL_C_FLAGS
		"-mbig-endian -mthumb -mthumb-interwork -nostdlib -fshort-wchar -fshort-enums -fpack-struct=4 -fno-builtin -fvisibility=hidden")
	set(P2K_GENERAL_CXX_FLAGS
		"-mbig-endian -mthumb -mthumb-interwork -nostdlib -fshort-wchar -fshort-enums -fpack-struct=4 -fno-builtin -fvisibility=hidden")
	set(P2K_GENERAL_ASM_FLAGS
		"-mbig-endian -mthumb -mthumb-interwork -nostdlib -fshort-wchar -fshort-enums -fpack-struct=4 -fno-builtin -fvisibility=hidden")
	set(P2K_GENERAL_C_LINK_FLAGS
		"${P2K_GENERAL_OPTIMIZATION_FLAGS} -pie -EB -nostdlib --allow-multiple-definition")
	set(P2K_GENERAL_CXX_LINK_FLAGS
		"${P2K_GENERAL_OPTIMIZATION_FLAGS} -pie -EB -nostdlib --allow-multiple-definition")
	set(P2K_GENERAL_LIBRARIES
		"${P2K_DIRECTORY_LIBRARIES}/${P2K_LIB_STD} ${P2K_DIRECTORY_LIBRARIES}/${P2K_LIB_C}")
elseif(CPU_MODE STREQUAL "arm")
	set(P2K_GENERAL_C_FLAGS
		"-mbig-endian -marm -mthumb-interwork -nostdlib -fshort-wchar -fshort-enums -fpack-struct=4 -fno-builtin -fvisibility=hidden")
	set(P2K_GENERAL_CXX_FLAGS
		"-mbig-endian -marm -mthumb-interwork -nostdlib -fshort-wchar -fshort-enums -fpack-struct=4 -fno-builtin -fvisibility=hidden")
	set(P2K_GENERAL_ASM_FLAGS
		"-mbig-endian -marm -mthumb-interwork -nostdlib -fshort-wchar -fshort-enums -fpack-struct=4 -fno-builtin -fvisibility=hidden")
	set(P2K_GENERAL_C_LINK_FLAGS
		"${P2K_GENERAL_OPTIMIZATION_FLAGS} -pie -EB -nostdlib --allow-multiple-definition")
	set(P2K_GENERAL_CXX_LINK_FLAGS
		"${P2K_GENERAL_OPTIMIZATION_FLAGS} -pie -EB -nostdlib --allow-multiple-definition")
	set(P2K_GENERAL_LIBRARIES
		"${P2K_DIRECTORY_LIBRARIES}/${P2K_LIB_STD} ${P2K_DIRECTORY_LIBRARIES}/${P2K_LIB_C}")
else()
	message(FATAL_ERROR "Please set '$CPU_MODE' to 'thumb' or 'arm'!")
endif()

set(CMAKE_C_FLAGS "${P2K_GENERAL_DEFINES_FLAGS} ${P2K_GENERAL_OPTIMIZATION_FLAGS} ${P2K_GENERAL_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${P2K_GENERAL_DEFINES_FLAGS} ${P2K_GENERAL_OPTIMIZATION_FLAGS} ${P2K_GENERAL_CXX_FLAGS}")
set(CMAKE_ASM_FLAGS "${P2K_GENERAL_ASM_FLAGS}")

set(CMAKE_C_LINK_EXECUTABLE
	"${CMAKE_C_LINK_EXECUTABLE} ${P2K_GENERAL_C_LINK_FLAGS} <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES> ${P2K_GENERAL_LIBRARIES} -o <TARGET>_raw.elf")
set(CMAKE_CXX_LINK_EXECUTABLE
	"${CMAKE_CXX_LINK_EXECUTABLE} ${P2K_GENERAL_CXX_LINK_FLAGS} <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES> ${P2K_GENERAL_LIBRARIES} -o <TARGET>_raw.elf")

#set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")

include_directories("${P2K_DIRECTORY_HEADERS_SDK}" "${P2K_DIRECTORY_HEADERS_LIBRARIES}")
