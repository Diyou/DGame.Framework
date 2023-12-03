# SDL2.cmake - DGame.Framework
#
# Copyright (c) 2023 Diyou
# All rights reserved.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(SDLDIR ${CACHE_DIR}/SDL2)
set(SDL_TAG release-2.28.5)

if(NOT
   EXISTS
   ${SDLDIR}
)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -c advice.detachedHead=false clone --depth 1
            --branch ${SDL_TAG} https://github.com/libsdl-org/SDL ${SDLDIR}
  )
endif()

execute_process(
  COMMAND ${GIT_EXECUTABLE} describe --tags
  WORKING_DIRECTORY ${SDLDIR}
  OUTPUT_VARIABLE SDL_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT
   ${SDL_TAG}
   STREQUAL
   ${SDL_VERSION}
)
  message(STATUS "Update SDL to: ${SDL_TAG}")
  execute_process(
    COMMAND ${GIT_EXECUTABLE} remote set-branches origin ${SDL_TAG}
    WORKING_DIRECTORY ${SDLDIR}
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} fetch --depth 1 origin ${SDL_TAG}
    WORKING_DIRECTORY ${SDLDIR}
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} checkout -f ${SDL_TAG}
    WORKING_DIRECTORY ${SDLDIR}
  )
endif()

add_subdirectory(${SDLDIR} EXCLUDE_FROM_ALL)
