cmake_minimum_required(VERSION 3.10)
project(VulkanLite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

add_library(VulkanLite src/VulkanLite.cpp)
target_include_directories(VulkanLite PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(VulkanLite PUBLIC Vulkan::Vulkan)

# Installation rules
install(TARGETS VulkanLite
    EXPORT VulkanLiteTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

export(TARGETS VulkanLite
    FILE VulkanLiteTargets.cmake
    NAMESPACE VulkanLite::
)

install(EXPORT VulkanLiteTargets
    FILE VulkanLiteTargets.cmake
    NAMESPACE VulkanLite::
    DESTINATION lib/cmake/VulkanLite
)

include(CMakePackageConfigHelpers)

# Generate version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/VulkanLiteConfigVersion.cmake"
    VERSION 1.0
    COMPATIBILITY AnyNewerVersion
)

# Generate config file with proper PACKAGE_INIT expansion
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/VulkanLiteConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/VulkanLiteConfig.cmake
    INSTALL_DESTINATION lib/cmake/VulkanLite
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/VulkanLiteConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/VulkanLiteConfigVersion.cmake"
    DESTINATION lib/cmake/VulkanLite
)