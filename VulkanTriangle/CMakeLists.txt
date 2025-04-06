cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET VulkanTriangle)

project(${TARGET})

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/inc/)

add_executable(${TARGET} main.cpp src/tcVulkanTriangleApp.cpp)
target_include_directories(${TARGET} PRIVATE ${PROJECT_SOURCE_DIR}/src)


target_link_libraries(${TARGET} Vulkan::Vulkan glfw)
