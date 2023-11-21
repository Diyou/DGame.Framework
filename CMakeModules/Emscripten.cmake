# This file needs to be included before project()
option(USE_EMSCRIPTEN "Use Emscripten toolchain" OFF)

if(${USE_EMSCRIPTEN})

if(DEFINED ENV{EMSDK})

set(EMSCRIPTEN_ROOT $ENV{EMSDK}/upstream/emscripten)
set(EMSCRIPTEN_TOOLCHAIN ${EMSCRIPTEN_ROOT}/cmake/Modules/Platform/Emscripten.cmake)

else()

set(DOWNLOAD_EMSCRIPTEN_VERSION 3.1.48)
set(ENV{EMSDK} ${CACHE_DIR}/emsdk)
set(EMSDK_COMMAND $ENV{EMSDK}/emsdk)
set(EMSCRIPTEN_ROOT $ENV{EMSDK}/upstream/emscripten)
set(EMSCRIPTEN_TOOLCHAIN ${EMSCRIPTEN_ROOT}/cmake/Modules/Platform/Emscripten.cmake)

macro(UpdateEmscripten)
        execute_process(COMMAND
                ${GIT_EXECUTABLE} pull
                WORKING_DIRECTORY $ENV{EMSDK}
        )
        execute_process(COMMAND
                ${EMSDK_COMMAND} install ${DOWNLOAD_EMSCRIPTEN_VERSION}
        )
        execute_process(COMMAND
                ${EMSDK_COMMAND} activate ${DOWNLOAD_EMSCRIPTEN_VERSION}
        )
endmacro()

if(NOT EXISTS ${EMSCRIPTEN_TOOLCHAIN})
        execute_process(COMMAND
                ${GIT_EXECUTABLE} clone https://github.com/emscripten-core/emsdk.git $ENV{EMSDK}
        )
        UpdateEmscripten()
endif()

include(${EMSCRIPTEN_TOOLCHAIN})

if(NOT ${DOWNLOAD_EMSCRIPTEN_VERSION} STREQUAL ${EMSCRIPTEN_VERSION})
        UpdateEmscripten()
endif()

endif(DEFINED ENV{EMSDK})

set(CMAKE_TOOLCHAIN_FILE ${EMSCRIPTEN_TOOLCHAIN})

### Define Emscripten Interface Libraries

# This target should not rely on SharedArrayBuffer
# See https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer#security_requirements
add_library(DGame.EMSlib INTERFACE)

# See https://github.com/emscripten-core/emscripten/blob/main/src/settings.js
target_link_options(DGame.EMSlib
INTERFACE
        -sUSE_WEBGPU
        -sASYNCIFY
        -sENVIRONMENT=web,worker
        -sWASM=1
        -sWASM_BIGINT=1
        --output_eol=linux
        #-sMEMORY64
$<$<CONFIG:Debug>:
        -fsanitize=undefined
        -sASSERTIONS=2
        -sSAFE_HEAP=1
        -Wno-limited-postlink-optimizations
>)
target_compile_options(DGame.EMSlib
INTERFACE
        -sUSE_SDL=2
        #-sMEMORY64
$<$<CONFIG:Debug>:
        -fsanitize=undefined
>)

# This target relies on SharedArrayBuffer for Multithreading
add_library(DGame.EMSlib_mt INTERFACE)
target_link_libraries(DGame.EMSlib_mt
INTERFACE 
        DGame.EMSlib
)
target_link_options(DGame.EMSlib_mt
INTERFACE
        #-sEXPORT_NAME=DGame
        #-sMODULARIZE=1
        -sOFFSCREENCANVAS_SUPPORT=1
        -sOFFSCREEN_FRAMEBUFFER=1
        -sUSE_PTHREADS=1
        -sPROXY_TO_PTHREAD=1
        -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency
        -pthread
        -sINVOKE_RUN=0
$<$<CONFIG:Debug>:      
        --emrun
>)
target_compile_options(DGame.EMSlib_mt
INTERFACE
        -pthread
)

string(APPEND CMAKE_C_FLAGS " -D_DEFAULT_SOURCE=1")

endif(${USE_EMSCRIPTEN})