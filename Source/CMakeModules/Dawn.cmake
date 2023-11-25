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

set(DAWN_DIR ${CACHE_DIR}/dawn)
set(DAWN_TAG chromium/6130)

if(NOT EXISTS ${DAWN_DIR})

execute_process(
COMMAND
    ${GIT_EXECUTABLE} clone --depth 1 --branch ${DAWN_TAG} https://dawn.googlesource.com/dawn ${DAWN_DIR}
)

set(DAWN_FETCH_DEPENDENCIES ON)
endif()

execute_process(
COMMAND 
    ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${DAWN_DIR}
    OUTPUT_VARIABLE DAWN_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT ${DAWN_TAG} STREQUAL ${DAWN_VERSION})
message("Update Dawn to: ${DAWN_TAG}")
execute_process(
COMMAND
    ${GIT_EXECUTABLE} remote set-branches origin ${DAWN_TAG}
    WORKING_DIRECTORY ${DAWN_DIR}
)
execute_process(
COMMAND
    ${GIT_EXECUTABLE} fetch --depth 1 origin ${DAWN_TAG}
    WORKING_DIRECTORY ${DAWN_DIR}
)
execute_process(
COMMAND
    ${GIT_EXECUTABLE} checkout -f ${DAWN_TAG}
    WORKING_DIRECTORY ${DAWN_DIR}
)

set(DAWN_FETCH_DEPENDENCIES ON)
endif()

set(DAWN_ENABLE_DESKTOP_GL OFF)
set(DAWN_ENABLE_OPENGLES OFF)
set(BUILD_SAMPLES OFF)
if(${EMSCRIPTEN})
    set(DAWN_USE_X11 OFF)
    set(DAWN_USE_WAYLAND OFF)
elseif(UNIX)
    set(DAWN_USE_X11 ON)
    set(DAWN_USE_WAYLAND ON)
endif()

add_subdirectory(${DAWN_DIR} EXCLUDE_FROM_ALL)
