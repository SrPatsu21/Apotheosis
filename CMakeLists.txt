cmake_minimum_required(VERSION 3.10)

# ============================================================
# Project
# ============================================================
set(PROJECT_NAME "Apotheosis")
project(${PROJECT_NAME} LANGUAGES CXX)

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
# Output directories
# ============================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

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

target_link_libraries(${PROJECT_NAME}_server
    game_common
)

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

# ============================================================
# Vulkan (CLIENT ONLY)
# ============================================================
if(UNIX)
    find_package(Vulkan REQUIRED)
elseif(WIN32)
    set(VULKAN_SDK_PATH "./lib/vulkan-sdk-win")
endif()

# ============================================================
# GLFW
# ============================================================
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_INSTALL OFF)

add_subdirectory(lib/glfw)

# ============================================================
# Dear ImGui
# ============================================================
set(IMGUI_DIR lib/Dear-ImGui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

# ============================================================
# GLM
# ============================================================
if(WIN32)
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE lib/glm/glm-1.0.1)
else()
    find_package(glm REQUIRED)
endif()

# ============================================================
# stb_image
# ============================================================
set(STB_DIR lib/stb)
set(STB_SOURCES src/client/image/stb_image_impl.cpp)

# ============================================================
# Assimp
# ============================================================
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)

add_subdirectory(lib/assimp)

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
        COMMAND ${GLSLANG_VALIDATOR} -V --target-env vulkan1.3 -S ${STAGE} ${SHADER} -o ${SPV}
        DEPENDS ${SHADER}
    )

    list(APPEND SHADER_OUTPUTS ${SPV})
endforeach()

add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})

# ============================================================
# Copy textures (CLIENT ONLY)
# ============================================================
set(TEXTURE_SRC_DIR ${CMAKE_SOURCE_DIR}/src/client/assets/textures)
set(TEXTURE_DST_DIR ${CMAKE_BINARY_DIR}/textures)

add_custom_target(Textures ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TEXTURE_DST_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${TEXTURE_SRC_DIR}
            ${TEXTURE_DST_DIR}
)

# ============================================================
# Copy models (CLIENT ONLY)
# ============================================================
set(MODELS_SRC_DIR ${CMAKE_SOURCE_DIR}/src/client/assets/models)
set(MODELS_DST_DIR ${CMAKE_BINARY_DIR}/models)

add_custom_target(Models ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MODELS_DST_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${MODELS_SRC_DIR}
            ${MODELS_DST_DIR}
)

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

add_dependencies(${PROJECT_NAME}_client Shaders Textures)

# ============================================================
# Client include directories
# ============================================================
target_include_directories(${PROJECT_NAME}_client PRIVATE
    src/client
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${STB_DIR}
)

if(WIN32)
    target_include_directories(${PROJECT_NAME}_client PRIVATE
        ${VULKAN_SDK_PATH}/Include
    )
endif()

# ============================================================
# Client compile definitions
# ============================================================
target_compile_definitions(${PROJECT_NAME}_client PRIVATE
    VK_PROTOTYPES
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME}_client PRIVATE
        NDEBUG
    )
endif()

if(WIN32)
    target_compile_definitions(${PROJECT_NAME}_client PRIVATE
        VK_USE_PLATFORM_WIN32_KHR
    )
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME}_client PRIVATE
        VK_USE_PLATFORM_XCB_KHR
    )
endif()

# ============================================================
# Client libraries
# ============================================================
if(WIN32)
    target_link_directories(${PROJECT_NAME}_client PRIVATE
        ${VULKAN_SDK_PATH}/Lib
    )

    target_link_libraries(${PROJECT_NAME}_client
        game_common
        glfw
        assimp
        vulkan-1
        glm
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME}_client
        game_common
        glfw
        Vulkan::Vulkan
        glm::glm
        assimp
        pthread
        dl
    )
endif()

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
