cmake_minimum_required(VERSION 3.10)

# ============================================================
# Project
# ============================================================
set(PROJECT_NAME "ProjectD")
project(${PROJECT_NAME})

# ============================================================
# C++ Standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# Default build type
# ============================================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# ============================================================
# Global compiler definitions
# ============================================================
add_compile_definitions(VK_PROTOTYPES)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(NDEBUG DISABLE_VALIDATION_LAYERS)
endif()

# ============================================================
# Output directories
# ============================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ============================================================
# Platform-specific Vulkan definitions
# ============================================================
if(WIN32)
    add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
elseif(UNIX)
    add_compile_definitions(VK_USE_PLATFORM_XCB_KHR)
endif()

# ============================================================
# ---------------- COMMON LIBRARY ------------------------------
# Shared code between client and server
# ============================================================
file(GLOB_RECURSE COMMON_SOURCES
    src/common/*.cpp
)

add_library(game_common STATIC ${COMMON_SOURCES})
target_include_directories(game_common PUBLIC src/common)

# ============================================================
# ---------------- SERVER EXECUTABLE ---------------------------
# Headless, no graphics, container-friendly
# ============================================================
file(GLOB_RECURSE SERVER_SOURCES
    src/server/*.cpp
)

add_executable(${PROJECT_NAME}_server ${SERVER_SOURCES})

if(UNIX)
    target_link_libraries(${PROJECT_NAME}_server
        pthread
        dl
    )
endif()

# ============================================================
# ---------------- CLIENT EXECUTABLE ---------------------------
# Graphics, input, UI, shaders
# ============================================================

# ---------------- Vulkan ----------------
if(UNIX)
    find_package(Vulkan REQUIRED)
elseif(WIN32)
    set(VULKAN_SDK_PATH "./lib/vulkan-sdk-win")
    include_directories(${VULKAN_SDK_PATH}/Include)
    link_directories(${VULKAN_SDK_PATH}/Lib)
endif()

# ---------------- GLFW ----------------
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_INSTALL OFF)

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

# ---------------- Dear ImGui ----------------
set(IMGUI_DIR lib/Dear-ImGui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

# ---------------- GLM ----------------
if(WIN32)
    include_directories(lib/glm/glm-1.0.1)
else()
    find_package(glm REQUIRED)
endif()

# ---------------- stb_image ----------------
set(STB_DIR lib/stb)
include_directories(${STB_DIR})
set(STB_SOURCES src/client/image/stb_image_impl.cpp)

# ---------------- Assimp ----------------
add_subdirectory(lib/assimp)
include_directories(lib/assimp/include)

# ============================================================
# Shader compilation (CLIENT ONLY)
# ============================================================
find_program(GLSLANG_VALIDATOR glslangValidator)
if(NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found")
endif()

set(SHADER_DIR ${CMAKE_SOURCE_DIR}/src/client/assets/shaders)
set(COMPILED_SHADER_DIR ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${COMPILED_SHADER_DIR})

set(SHADERS
    vertex.glsl
    fragment.glsl
)

set(SHADER_OUTPUTS "")
foreach(SHADER_NAME IN LISTS SHADERS)
    set(SHADER ${SHADER_DIR}/${SHADER_NAME})
    set(SPV ${COMPILED_SHADER_DIR}/${SHADER_NAME}.spv)

    if(SHADER_NAME MATCHES "vertex")
        set(STAGE vert)
    elseif(SHADER_NAME MATCHES "fragment")
        set(STAGE frag)
    endif()

    add_custom_command(
        OUTPUT ${SPV}
        COMMAND ${GLSLANG_VALIDATOR} -V -S ${STAGE} ${SHADER} -o ${SPV}
        DEPENDS ${SHADER}
    )

    list(APPEND SHADER_OUTPUTS ${SPV})
endforeach()

add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})

# ============================================================
# Client sources & executable
# ============================================================
file(GLOB_RECURSE CLIENT_SOURCES
    src/client/*.cpp
)

add_executable(${PROJECT_NAME}_client
    ${CLIENT_SOURCES}
    ${IMGUI_SOURCES}
    ${STB_SOURCES}
)

add_dependencies(${PROJECT_NAME}_client Shaders)

# ============================================================
# Client libraries
# ============================================================
if(WIN32)
    set(CLIENT_LIBS glfw assimp vulkan-1)
elseif(UNIX)
    set(CLIENT_LIBS
        glfw
        Vulkan::Vulkan
        X11 Xrandr Xi Xcursor
        glm::glm
        assimp
        pthread
        dl
    )
endif()

target_link_libraries(${PROJECT_NAME}_client
    game_common
    ${CLIENT_LIBS}
)

# ============================================================
# Mods directory creation (no target dependency)
# ============================================================
set(MODS_DIR ${CMAKE_BINARY_DIR}/mods)

add_custom_target(CreateModsDir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${MODS_DIR}/common
        ${MODS_DIR}/client
        ${MODS_DIR}/server
)

# ============================================================
# Output names
# ============================================================
set_target_properties(${PROJECT_NAME}_client PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_client")
set_target_properties(${PROJECT_NAME}_server PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_server")