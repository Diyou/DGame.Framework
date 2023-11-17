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