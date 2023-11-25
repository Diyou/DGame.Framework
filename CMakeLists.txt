# MIT License
# 
# Copyright (c) 2023 Diyou
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.24)

# Setup Environment first
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Source/CMakeModules)

set(CACHE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.cache)

find_package(Git REQUIRED)

include(Emscripten) # Needs to be included before project()
project(DGame.Framework)

include(CheckIPOSupported)
check_ipo_supported(RESULT IPOSupported OUTPUT error)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${IPOSupported})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Define WASM Build Targets
if(${EMSCRIPTEN})
set(CMAKE_EXECUTABLE_SUFFIX .html)

# Run python3 -m http.server --directory ${CMAKE_BINARY_DIR}/dist
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

add_library(DGame.Framework STATIC
        Source/Backends/Browser.cpp
)

target_link_options(DGame.Framework
INTERFACE
        --shell-file=${CMAKE_CURRENT_LIST_DIR}/Source/Shell.html
)
target_link_libraries(DGame.Framework
PRIVATE 
        DGame.EMSlib
)
else(${EMSCRIPTEN})
include(Dawn)
include(SDL2)
# Define Native Build Targets
add_library(DGame.Framework
        Source/Backends/Native.cpp
)

set_target_properties(DGame.Framework
PROPERTIES
        PREFIX ""
)

target_link_libraries(DGame.Framework
PUBLIC  webgpu_cpp 
        webgpu_dawn
        SDL2::SDL2
)

endif(${EMSCRIPTEN})

target_include_directories(DGame.Framework
PUBLIC 
        Header
)
target_sources(DGame.Framework
PRIVATE
        Source/Backends/SDLWindow.cpp
)

target_compile_definitions(DGame.Framework
PUBLIC
        main=__main__
)

set(INTERMEDIATE_HEADERS ${CMAKE_BINARY_DIR}/Intermediate)
include(Shaders)

target_precompile_headers(DGame.Framework
PUBLIC
        ${WGSL_INTERMEDIATE_HEADER}
        <cstdlib>
        <string>       
)

add_subdirectory(Examples EXCLUDE_FROM_ALL)
