# CMakeLists.txt - DGame.Framework
#
# Copyright (c) 2023 Diyou
# All rights reserved.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.21)

option(BUILD_SHARED_LIBS "Link dynamically" ON)
option(BUILD_WASM "Compile to WASM with Emscripten" OFF)
option(ENABLE_IPO "If supported use Interprocedural Optimization" ON)

set(CMAKE_POLICY_DEFAULT_CMP0148 "OLD") # PythonInterp
set(CMAKE_POLICY_DEFAULT_CMP0069 "NEW") # IPO

# Setup Environment first
list(
  APPEND
  CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/CMakeModules
)
set(CACHE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.cache)

find_package(Git REQUIRED)

include(Emscripten) # Needs to be included before project()
project(DGame.Framework)

if(${ENABLE_IPO} AND NOT ${BUILD_WASM})
  include(CheckIPOSupported)
  check_ipo_supported(RESULT IPOSupported OUTPUT error)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${IPOSupported})
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_MACOSX_BUNDLE ON)

# Example values
set(MACOSX_BUNDLE_GUI_IDENTIFIER io.DGame.Framework)
set(MACOSX_BUNDLE_BUNDLE_NAME DGame.Framework)
set(MACOSX_BUNDLE_BUNDLE_VERSION 0)
set(MACOSX_BUNDLE_SHORT_VERSION_STRING 0.0.1)
set(MACOSX_BUNDLE_LONG_VERSION_STRING 0.0.1)
set(MACOSX_BUNDLE_INFO_STRING
    "DGame.Framework version ${MACOSX_BUNDLE_SHORT_VERSION_STRING}"
)

include(glm)
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
if(${BUILD_WASM})
  set(CMAKE_EXECUTABLE_SUFFIX .html)

  add_library(DGame.Framework STATIC Source/Backends/Browser.cpp)

  target_link_options(
    DGame.Framework
    INTERFACE --shell-file=${CMAKE_CURRENT_LIST_DIR}/Source/Shell.html
  )
  target_link_libraries(
    DGame.Framework
    PUBLIC DGame.EMSlib
    INTERFACE glm::glm
  )
else(${BUILD_WASM})
  set(Boost_USE_STATIC_LIBS ON)
  find_package(Boost REQUIRED COMPONENTS thread)
  include(Dawn)
  include(SDL2)

  add_library(DGame.Framework Source/Backends/Native.cpp Source/ThreadPool.cpp)

  target_link_libraries(
    DGame.Framework
    PUBLIC webgpu_cpp
           webgpu_dawn
           SDL2::SDL2
           Boost::thread
    INTERFACE glm::glm
  )
  #install(FILES $<TARGET_RUNTIME_DLLS:DGame.Framework> TYPE BIN)
endif(${BUILD_WASM})

set_target_properties(DGame.Framework PROPERTIES PREFIX "")

target_include_directories(DGame.Framework PUBLIC Header)

target_sources(DGame.Framework PRIVATE Source/Backends/SDLWindow.cpp)

set(INTERMEDIATE_HEADERS ${CMAKE_BINARY_DIR}/Intermediate)
include(Shaders)

target_precompile_headers(
  DGame.Framework
  PUBLIC ${WGSL_INTERMEDIATE_HEADER}
         $<$<BOOL:${WIN32}>:SDKDDKVer.h>
         <cstdlib>
         <memory>
         <iostream>
         <string>
)

add_subdirectory(Examples EXCLUDE_FROM_ALL)
