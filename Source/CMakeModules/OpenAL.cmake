# OpenAL.cmake - DGame.Framework
#
# Copyright (c) 2023 Diyou
# All rights reserved.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(OPENALDIR ${CACHE_DIR}/OpenAL)
set(OPENAL_TAG 1.23.1)

if(NOT
   EXISTS
   ${OPENALDIR}
)
  execute_process(
    COMMAND
      ${GIT_EXECUTABLE} -c advice.detachedHead=false clone --depth 1 --branch
      ${OPENAL_TAG} https://github.com/kcat/openal-soft ${OPENALDIR}
  )
endif()

execute_process(
  COMMAND ${GIT_EXECUTABLE} describe --tags
  WORKING_DIRECTORY ${OPENALDIR}
  OUTPUT_VARIABLE OPENAL_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT
   ${OPENAL_TAG}
   STREQUAL
   ${OPENAL_VERSION}
)
  message(STATUS "Update OpenAL to: ${OPENAL_TAG}")
  execute_process(
    COMMAND ${GIT_EXECUTABLE} remote set-branches origin ${OPENAL_TAG}
    WORKING_DIRECTORY ${OPENALDIR}
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} fetch --depth 1 origin ${OPENAL_TAG}
    WORKING_DIRECTORY ${OPENALDIR}
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} checkout -f ${OPENAL_TAG}
    WORKING_DIRECTORY ${OPENALDIR}
  )
endif()

add_subdirectory(${OPENALDIR} EXCLUDE_FROM_ALL)
