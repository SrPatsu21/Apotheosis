cmake_minimum_required(VERSION 3.10)
project(ProjectD)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add Vulkan
find_package(Vulkan REQUIRED)

# GLFW setup
add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

#* Dear ImGui setup
set(IMGUI_DIR lib/Dear-ImGui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

add_library(Dear-ImGui STATIC ${IMGUI_SOURCES})

target_include_directories(Dear-ImGui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)


# Add source files
set(SOURCES
    src/main.cpp
    src/VulkanApp.cpp
    src/VulkanApp.hpp
)

# Create the executable
add_executable(ProjectD ${SOURCES})
set_target_properties(
    ProjectD
    PROPERTIES
        OUTPUT_NAME "ProjectD"
        SUFFIX ".out"
)

# Link libraries
target_link_libraries(ProjectD Vulkan::Vulkan glfw Dear-ImGui)