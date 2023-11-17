cmake_minimum_required(VERSION 3.24)

# Setup Environment first
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

set(CACHE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/.cache)

set(FETCHCONTENT_QUIET OFF)
find_package(Git REQUIRED)

include(FetchContent)
include(Emscripten) # Needs to be included before project()

# Define Build Targets
project(DGame.Framework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(DGame.Framework)
target_include_directories(DGame.Framework PUBLIC Header)
target_sources(DGame.Framework PRIVATE
        Source/Runtime.cpp
)

if(${EMSCRIPTEN})
target_sources(DGame.Framework
PRIVATE
        Source/Backends/Browser.cpp
)
set_target_properties(DGame.Framework
PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist
        SUFFIX .html
)
# Run python3 -m http.server --directory ${CMAKE_BINARY_DIR}/dist
target_link_options(DGame.Framework
PRIVATE
        --shell-file=${CMAKE_CURRENT_LIST_DIR}/Source/Shell.html
)
else()
include(Dawn)
include(SDL2)
target_sources(DGame.Framework
PRIVATE 
        Source/Backends/Native.cpp
        Source/Backends/SDLWindow.cpp
)

target_link_libraries(DGame.Framework
PRIVATE webgpu_cpp 
        webgpu_dawn
        SDL2::SDL2
)
endif()

set(INTERMEDIATE_HEADERS ${CMAKE_BINARY_DIR}/Intermediate)
include(Shaders)

target_precompile_headers(DGame.Framework
PUBLIC
        ${WGSL_INTERMEDIATE_HEADER}
)

#include_directories(${INTERMEDIATE_HEADERS})
