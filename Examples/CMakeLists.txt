# Examples/CMakeLists.txt - DGame.Framework
#
# Copyright (c) 2023 Diyou
# All rights reserved.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_executable(HelloTriangle MACOSX_BUNDLE HelloTriangle.cpp)
target_link_libraries(HelloTriangle PRIVATE DGame.Framework)

set_target_properties(
  HelloTriangle PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME HelloTriangle
)
