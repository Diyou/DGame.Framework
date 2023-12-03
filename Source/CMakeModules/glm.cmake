# glm.cmake - DGame.Framework
#
# Copyright (c) 2023 Diyou
# All rights reserved.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(GLMDIR ${CACHE_DIR}/glm)
set(GLM_TAG 0.9.9.8)

if(NOT
   EXISTS
   ${GLMDIR}
)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -c advice.detachedHead=false clone --depth 1
            --branch ${GLM_TAG} https://github.com/g-truc/glm ${GLMDIR}
  )
endif()
execute_process(
  COMMAND ${GIT_EXECUTABLE} describe --tags
  WORKING_DIRECTORY ${GLMDIR}
  OUTPUT_VARIABLE GLM_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT
   ${GLM_TAG}
   STREQUAL
   ${GLM_VERSION}
)
  message(STATUS "Update glm to: ${GLM_TAG}")
  execute_process(
    COMMAND ${GIT_EXECUTABLE} remote set-branches origin ${GLM_TAG}
    WORKING_DIRECTORY ${GLMDIR}
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} fetch --depth 1 origin ${GLM_TAG}
    WORKING_DIRECTORY ${GLMDIR}
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} checkout -f ${GLM_TAG}
    WORKING_DIRECTORY ${GLMDIR}
  )
endif()

add_subdirectory(${GLMDIR} EXCLUDE_FROM_ALL)
