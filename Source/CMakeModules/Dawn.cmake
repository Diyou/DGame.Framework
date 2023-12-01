# Dawn.cmake - DGame.Framework
#
# Copyright (c) 2023 Diyou
# All rights reserved.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(DAWN_DIR ${CACHE_DIR}/dawn)
set(DAWN_TAG chromium/6130)

if(NOT
   EXISTS
   ${DAWN_DIR}
)

  execute_process(
    COMMAND ${GIT_EXECUTABLE} clone --depth 1 --branch ${DAWN_TAG}
            https://dawn.googlesource.com/dawn ${DAWN_DIR}
  )

  set(DAWN_FETCH_DEPENDENCIES ON)
endif()

execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${DAWN_DIR}
  OUTPUT_VARIABLE DAWN_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT
   ${DAWN_TAG}
   STREQUAL
   ${DAWN_VERSION}
)
  message("Update Dawn to: ${DAWN_TAG}")
  execute_process(
    COMMAND ${GIT_EXECUTABLE} remote set-branches origin ${DAWN_TAG}
    WORKING_DIRECTORY ${DAWN_DIR}
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} fetch --depth 1 origin ${DAWN_TAG}
    WORKING_DIRECTORY ${DAWN_DIR}
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} checkout -f ${DAWN_TAG}
    WORKING_DIRECTORY ${DAWN_DIR}
  )

  set(DAWN_FETCH_DEPENDENCIES ON)
endif()

set(DAWN_ENABLE_DESKTOP_GL OFF)
set(DAWN_ENABLE_OPENGLES OFF)
set(DAWN_ENABLE_NULL OFF)
set(DAWN_BUILD_SAMPLES OFF)
if(${EMSCRIPTEN})
  set(DAWN_USE_X11 OFF)
  set(DAWN_USE_WAYLAND OFF)
elseif(UNIX AND NOT APPLE)
  set(DAWN_USE_X11 ON)
  set(DAWN_USE_WAYLAND ON)
endif()

add_subdirectory(${DAWN_DIR} EXCLUDE_FROM_ALL)
