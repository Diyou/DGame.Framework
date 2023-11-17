# This file needs to be included before project()
option(USE_EMSCRIPTEN "Use Emscripten toolchain" OFF)

if(${USE_EMSCRIPTEN})

set(DOWNLOAD_EMSCRIPTEN_VERSION 3.1.47)
set(ENV{EMSDK} ${CACHE_DIR}/emsdk)
set(EMSDK $ENV{EMSDK}/emsdk)
set(EMSCRIPTEN_ROOT $ENV{EMSDK}/upstream/emscripten)
set(EMSCRIPTEN_TOOLCHAIN ${EMSCRIPTEN_ROOT}/cmake/Modules/Platform/Emscripten.cmake)
include(${EMSCRIPTEN_TOOLCHAIN})

if(NOT EXISTS ${EMSCRIPTEN_TOOLCHAIN})
        execute_process(COMMAND
                ${GIT_EXECUTABLE} clone https://github.com/emscripten-core/emsdk.git $ENV{EMSDK}
        )
endif()
if(NOT ${DOWNLOAD_EMSCRIPTEN_VERSION} STREQUAL ${EMSCRIPTEN_VERSION})
        execute_process(COMMAND
                ${GIT_EXECUTABLE} pull
                WORKING_DIRECTORY $ENV{EMSDK}
        )
        execute_process(COMMAND
                ${EMSDK} install ${DOWNLOAD_EMSCRIPTEN_VERSION}
        )
        execute_process(COMMAND
                ${EMSDK} activate ${DOWNLOAD_EMSCRIPTEN_VERSION}
        )
endif()

set(CMAKE_TOOLCHAIN_FILE ${EMSCRIPTEN_TOOLCHAIN})

add_link_options(
        #-sMIN_WEBGL_VERSION=2
        #-sMAX_WEBGL_VERSION=2
        -fsanitize=undefined
        -sOFFSCREENCANVAS_SUPPORT=1
        -sOFFSCREEN_FRAMEBUFFER=1
        -sENVIRONMENT=web,worker
        -sWASM=1
        -sUSE_WEBGPU
        #-sUSE_GLFW=3
        -sASYNCIFY
        -sASSERTIONS=2
        -sSAFE_HEAP=1
        -sUSE_PTHREADS=1
        #-sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency
        -sPROXY_TO_PTHREAD
        --emrun
        -sINVOKE_RUN=0
)

# -sMEMORY64 -pthread 
set(EMSCRIPTEN_FLAGS " -pthread  -sUSE_SDL=2 -O3 -s DISABLE_EXCEPTION_CATCHING=1")

string(APPEND CMAKE_CXX_FLAGS " ${EMSCRIPTEN_FLAGS}")
string(APPEND CMAKE_C_FLAGS " -D_DEFAULT_SOURCE=1 ${EMSCRIPTEN_FLAGS}")
string(APPEND CMAKE_EXE_LINKER_FLAGS " ${EMSCRIPTEN_FLAGS}  --output_eol=linux")

endif(${USE_EMSCRIPTEN})