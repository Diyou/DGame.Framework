# SDL2.cmake - DGame.Framework
# 
# Copyright (c) 2023 Diyou
# All rights reserved.
# 
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

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
