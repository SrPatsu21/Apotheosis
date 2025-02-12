cmake_minimum_required(VERSION 3.10)
project(ProjectD)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")
set(CMAKE_PREFIX_PATH
# LINUX SPECIFIC
    ${LINUX_VENDOR_BINARIES_DIR}/vulkan-loader
    # PLATFORM AGNOSTIC
    ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/vulkan-headers
)

# Define platform-specific compiler flags
if(WIN32)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
elseif(UNIX)
    add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
endif()

# Force debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# Add Vulkan
find_package(Vulkan REQUIRED)
find_package(VulkanLoader REQUIRED)
find_package(VulkanHeaders REQUIRED CONFIG)

# GLFW setup
add_subdirectory(lib/glfw)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
# add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(lib/glfw/include)

#* Dear ImGui setup
set(IMGUI_DIR lib/Dear-ImGui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

# Libraries
find_package(Vulkan REQUIRED)
#find_library(VULKAN_LIBRARY
    #NAMES vulkan vulkan-1)
#set(LIBRARIES "glfw;${VULKAN_LIBRARY}")

# Link libraries
if(WIN32)
    set(LIBRARIES "glfw;Vulkan::Headers;Vulkan::Loader")
elseif(UNIX)
    set(LIBRARIES "glfw;Vulkan::Headers;Vulkan::Loader;pthread;dl;X11;Xrandr;Xi;Xcursor")
endif()

# Use vulkan headers from glfw:
include_directories(${GLFW_DIR}/deps)

# Add source files
file(GLOB SOURCES src/*.cpp)

# Create the executable
add_executable(ProjectD ${SOURCES} ${IMGUI_SOURCES})
if(WIN32)
    set_target_properties(
        ProjectD
        PROPERTIES
            OUTPUT_NAME "ProjectD"
            SUFFIX ".exe"
    )
elseif(UNIX)
    set_target_properties(
        ProjectD
        PROPERTIES
            OUTPUT_NAME "ProjectD"
            SUFFIX ".out"
    )
endif()

# Link libraries
target_link_libraries(ProjectD ${LIBRARIES})