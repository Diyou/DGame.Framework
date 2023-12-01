# Emscripten.cmake - DGame.Framework
#
# Copyright (c) 2023 Diyou
# All rights reserved.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# This file needs to be included before project()

if(${BUILD_WASM})

  if(DEFINED ENV{EMSDK})

    set(EMSCRIPTEN_ROOT $ENV{EMSDK}/upstream/emscripten)
    set(EMSCRIPTEN_TOOLCHAIN
        ${EMSCRIPTEN_ROOT}/cmake/Modules/Platform/Emscripten.cmake
    )

  else()

    set(DOWNLOAD_EMSCRIPTEN_VERSION 3.1.48)
    set(ENV{EMSDK} ${CACHE_DIR}/emsdk)
    set(EMSDK_COMMAND $ENV{EMSDK}/emsdk)
    set(EMSCRIPTEN_ROOT $ENV{EMSDK}/upstream/emscripten)
    set(EMSCRIPTEN_TOOLCHAIN
        ${EMSCRIPTEN_ROOT}/cmake/Modules/Platform/Emscripten.cmake
    )

    macro(UpdateEmscripten)
      execute_process(
        COMMAND ${GIT_EXECUTABLE} pull WORKING_DIRECTORY $ENV{EMSDK}
      )
      execute_process(
        COMMAND ${EMSDK_COMMAND} install ${DOWNLOAD_EMSCRIPTEN_VERSION}
      )
      execute_process(
        COMMAND ${EMSDK_COMMAND} activate ${DOWNLOAD_EMSCRIPTEN_VERSION}
      )
    endmacro()

    if(NOT
       EXISTS
       ${EMSCRIPTEN_TOOLCHAIN}
    )
      execute_process(
        COMMAND ${GIT_EXECUTABLE} clone
                https://github.com/emscripten-core/emsdk.git $ENV{EMSDK}
      )
      updateemscripten()
    endif()

    include(${EMSCRIPTEN_TOOLCHAIN})

    if(NOT
       ${DOWNLOAD_EMSCRIPTEN_VERSION}
       STREQUAL
       ${EMSCRIPTEN_VERSION}
    )
      updateemscripten()
    endif()

  endif(DEFINED ENV{EMSDK})

  set(CMAKE_TOOLCHAIN_FILE ${EMSCRIPTEN_TOOLCHAIN})

  # Define Emscripten Interface Libraries

  add_library(DGame.EMSlib.common INTERFACE)

  # See https://github.com/emscripten-core/emscripten/blob/main/src/settings.js
  target_link_options(
    DGame.EMSlib.common
    INTERFACE -sUSE_WEBGPU
              -sASYNCIFY
              -sWASM=1
              -sWASM_BIGINT=1
              -sNO_EXIT_RUNTIME=1
              -sINVOKE_RUN=0
              -sUSE_BOOST_HEADERS=1
              -sUSE_SDL=2
              -sDISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1
              --output_eol=linux
              -sALLOW_MEMORY_GROWTH=0
              # -sMEMORY64
              $<$<CONFIG:Debug>:
              -fsanitize=undefined
              # -sASSERTIONS=2 see
              # https://github.com/emscripten-core/emscripten/issues/19655
              -sASSERTIONS=1
              -sSAFE_HEAP=1
              -Wno-limited-postlink-optimizations
              --emrun
              >
  )
  target_compile_options(
    DGame.EMSlib.common
    INTERFACE -sUSE_BOOST_HEADERS=1
              -sUSE_SDL=2
              # -sMEMORY64
              $<$<CONFIG:Debug>:
              -fsanitize=undefined
              >
  )
  target_precompile_headers(DGame.EMSlib.common INTERFACE <emscripten.h>)

  # This target should not rely on SharedArrayBuffer See
  # https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer#security_requirements
  add_library(DGame.EMSlib INTERFACE)
  target_link_libraries(DGame.EMSlib INTERFACE DGame.EMSlib.common)

  target_link_options(
    DGame.EMSlib
    INTERFACE -sENVIRONMENT=web
              $<$<CONFIG:Debug>:
              -sMALLOC="emmalloc-memvalidate"
              >
              $<$<CONFIG:Release>:
              -sMALLOC="emmalloc"
              >
  )

  # This target relies on SharedArrayBuffer for Multithreading
  add_library(DGame.EMSlib_mt INTERFACE)
  target_link_libraries(DGame.EMSlib_mt INTERFACE DGame.EMSlib.common)
  target_link_options(
    DGame.EMSlib_mt
    INTERFACE # -sEXPORT_NAME=DGame -sMODULARIZE=1
              -sENVIRONMENT=web,worker
              -sOFFSCREENCANVAS_SUPPORT=1
              -sOFFSCREEN_FRAMEBUFFER=1
              -sUSE_PTHREADS=1
              -sPROXY_TO_PTHREAD=1
              -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency
              -pthread
              #-sMALLOC="emmalloc"
  )
  target_compile_options(DGame.EMSlib_mt INTERFACE -pthread)

  string(
    APPEND
    CMAKE_C_FLAGS
    " -D_DEFAULT_SOURCE=1"
  )

endif(${BUILD_WASM})
