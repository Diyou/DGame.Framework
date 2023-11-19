set(DAWN_DIR ${CACHE_DIR}/dawn)
set(DAWN_TAG chromium/6130)

if(NOT EXISTS ${DAWN_DIR})

execute_process(
COMMAND
    ${GIT_EXECUTABLE} clone --depth 1 --branch ${DAWN_TAG} https://dawn.googlesource.com/dawn ${DAWN_DIR}
)
file(WRITE ${CACHE_DIR}/dawn_version ${DAWN_TAG})
set(DAWN_FETCH_DEPENDENCIES ON)
endif()

file(STRINGS ${CACHE_DIR}/dawn_version DAWN_VERSION NEWLINE_CONSUME)

if(NOT ${DAWN_TAG} STREQUAL ${DAWN_VERSION})
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

file(WRITE ${CACHE_DIR}/dawn_version ${DAWN_TAG})
set(DAWN_FETCH_DEPENDENCIES ON)
endif()

set(DAWN_ENABLE_DESKTOP_GL OFF)
set(DAWN_ENABLE_OPENGLES OFF)
if(${EMSCRIPTEN})
    set(DAWN_USE_X11 OFF)
    set(DAWN_USE_WAYLAND OFF)
elseif(UNIX)
    set(DAWN_USE_X11 ON)
    set(DAWN_USE_WAYLAND ON)
endif()

add_subdirectory(${DAWN_DIR} EXCLUDE_FROM_ALL)