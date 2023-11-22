find_package(SDL2)

if(NOT ${SDL2_FOUND})
set(FETCHCONTENT_QUIET OFF)
include(FetchContent)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL
    GIT_TAG         release-2.28.5
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(SDL2)
endif()

