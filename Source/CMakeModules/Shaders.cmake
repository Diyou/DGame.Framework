# MIT License
# 
# Copyright (c) 2023 Diyou
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Process WGSL Shaders
file(GLOB_RECURSE wgsl_shaders Source/Shaders/*.wgsl)
foreach(wgsl_shader ${wgsl_shaders})

get_filename_component(shader_file ${wgsl_shader} NAME_WLE)
file(STRINGS ${wgsl_shader} wgsl_shader_string NEWLINE_CONSUME)

message("Importing WGSL Shader: ${shader_file}")

set(wgsl_constexpr "static constexpr char ${shader_file}[] = R\"(\n${wgsl_shader_string}\n)\"\;\n")
string(APPEND wgsl_constexpr_list ${wgsl_constexpr})

endforeach()

set(wgsl_shaders_intermediate_header
"#pragma once
namespace DGame::Shaders::WGSL{
${wgsl_constexpr_list}}"
)
set(WGSL_INTERMEDIATE_HEADER ${INTERMEDIATE_HEADERS}/Shaders/WGSLShaders.h)

set(wgsl_is_up_to_date false)
if(EXISTS ${WGSL_INTERMEDIATE_HEADER})
    file(STRINGS ${WGSL_INTERMEDIATE_HEADER} compare_string NEWLINE_CONSUME)
    string(REPLACE "\\;" "\;" compare_string ${compare_string})
    string(COMPARE EQUAL "${compare_string}" "${wgsl_shaders_intermediate_header}" wgsl_is_up_to_date)
endif()
if(NOT ${wgsl_is_up_to_date})
    file(WRITE ${WGSL_INTERMEDIATE_HEADER} "${wgsl_shaders_intermediate_header}")
endif()

# TODO Process SPIR-V Shaders
# see https://cmake.org/cmake/help/latest/module/FindVulkan.html
find_package(Vulkan COMPONENTS glslc)
if(${Vulkan_FOUND})
message("SDK: $ENV{VULKAN_SDK}")
message("Found Vulkan at ${Vulkan_LIBRARY}")
message("GLSC ${Vulkan_GLSLC_EXECUTABLE}")
endif()
