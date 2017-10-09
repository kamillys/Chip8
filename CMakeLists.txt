project(Chip8)
cmake_minimum_required(VERSION 2.8)

# Supported build types:
#  Debug
#  Release
#  RelWithDebInfo
#
# to set build type: -DCMAKE_BUILD_TYPE=...
#TODO: Write this properly
#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_BUILD_TYPE "Debug")

set(CHIP8_DEBUGGER_ENABLED ON CACHE BOOL "Enable integrated Chip8 debugger")


find_package(PkgConfig)
if(NOT PKG_CONFIG_FOUND)
    message(FATAL_ERROR "PkgConfig not found. Aborting")
endif()

# Enable C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    LIST(APPEND CMAKE_CXX_FLAGS " -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#if SDL
pkg_check_modules(SDL REQUIRED sdl2>=2.0.0)
if (SDL_FOUND)
    list(APPEND INCLUDEDIRS ${SDL_INCLUDE_DIRS})
    set(PROJECTLIBS ${PROJECTLIBS} ${SDL_LIBRARIES} )
    add_definitions(${SDL_CFLAGS} ${SDL_CFLAGS_OTHER})
endif() # SDL_FOUND

#if DEBUGGER
#if (CHIP8_DEBUGGER_ENABLED)
#    add_definitions(-DCHIP8_ENABLE_DEBUGGER)
    list(APPEND PROJECTLIBS "readline")
#endif()

#find_package(Boost COMPONENTS regex)
#if (Boost_FOUND)
#    set(INCLUDEDIRS ${INCLUDEDIRS} ${Boost_INCLUDE_DIRS})
#    set(PROJECTLIBS ${PROJECTLIBS} ${Boost_LIBRARIES} )
#endif()

#Add include directories
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)
include_directories(${INCLUDEDIRS})

#defines
#add_definitions(-DUSE_HUGE_GPU_MEM)

#Project sources
set(SRC_LIST
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/instruction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/audio.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/board.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/memory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/debugger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/debugger.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sdlvideo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sdlvideo.h"
    )

#file(GLOB_RECURSE HEADERS_LIST ${CMAKE_CURRENT_SOURCE_DIR}/includes/*)

set(HEADERS_LIST
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/Chip8/Audio.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/Chip8/Board.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/Chip8/Common.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/Chip8/Cpu.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/Chip8/Memory.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/Chip8/Video.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/includes/Chip8/Instruction.h"
    )

#Library
add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADERS_LIST})
target_link_libraries(${PROJECT_NAME} ${PROJECTLIBS})

#Installation
#message("Installation dir: ${CMAKE_INSTALL_PREFIX}")

# TODO: Consider: macro? for each PC file.
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/cucs.pc.in ${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/cucs.pc @ONLY)
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/cucs.pc DESTINATION lib/pkgconfig)

#include(GNUInstallDirs)
#install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
#install(FILES ${HEADERS_LIST} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
