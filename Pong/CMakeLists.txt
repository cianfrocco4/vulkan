cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET Pong)

project(${TARGET})

find_package(VulkanLite CONFIG REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# Define source files
set(SOURCES main.cpp)

# Define shader files
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADERS
    ${SHADER_DIR}/triangle.vert
    ${SHADER_DIR}/triangle.frag
)

# Find glslc from Vulkan SDK
find_program(GLSLC glslc HINTS $ENV{VULKAN_SDK}/bin)
if (NOT GLSLC)
    message(FATAL_ERROR "glslc not found. Please ensure Vulkan SDK is installed and VULKAN_SDK environment variable is set.")
endif()

# Compile shaders to SPIR-V
foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SHADER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv)
    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND ${GLSLC} ${SHADER} -o ${SHADER_OUTPUT}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER_NAME} to SPIR-V"
    )
    list(APPEND SHADER_OUTPUTS ${SHADER_OUTPUT})
endforeach()

# Custom target to ensure shaders are built
add_custom_target(CompileShaders ALL DEPENDS ${SHADER_OUTPUTS})

# Add executable
add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} VulkanLite glfw)

# Copy shaders to binary directory (optional, if runtime access is needed)
#foreach(SHADER_OUTPUT ${SHADER_OUTPUTS})
#    get_filename_component(SHADER_NAME ${SHADER_OUTPUT} NAME)
#    configure_file(${SHADER_OUTPUT} ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME} COPYONLY)
#endforeach()
