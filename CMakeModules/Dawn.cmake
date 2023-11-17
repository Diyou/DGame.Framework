set(DAWN_DIR ${CACHE_DIR}/dawn)
set(DAWN_TAG chromium/6104)

if(NOT EXISTS ${DAWN_DIR})

execute_process(COMMAND
    ${GIT_EXECUTABLE} clone --branch ${DAWN_TAG} https://dawn.googlesource.com/dawn ${DAWN_DIR}
)

endif()

execute_process(COMMAND 
    ${GIT_EXECUTABLE} describe --tags
    WORKING_DIRECTORY ${DAWN_DIR}
    OUTPUT_VARIABLE DAWN_VERSION
)
message("Dawn: ${DAWN_VERSION}")

set(DAWN_FETCH_DEPENDENCIES ON)

if(${EMSCRIPTEN})
    set(DAWN_USE_X11 OFF)
    set(DAWN_USE_WAYLAND OFF)
elseif(UNIX)
    set(DAWN_USE_X11 ON)
    set(DAWN_USE_WAYLAND ON)
endif()

add_subdirectory(${DAWN_DIR} EXCLUDE_FROM_ALL)