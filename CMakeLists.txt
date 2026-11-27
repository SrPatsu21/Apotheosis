cmake_minimum_required(VERSION 3.10)

# ============================================================
# Project
# ============================================================
set(PROJECT_NAME "Apotheosis")
project(
    ${PROJECT_NAME}
    VERSION 1.0.0
    LANGUAGES C CXX
)

# ============================================================
# C++ Standard
# ============================================================
# C++26 needs GCC 14+. The MinGW-w64 compiler shipped with the image is older
# (GCC 13), so Windows builds fall back to C++23 automatically.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
    set(CMAKE_CXX_STANDARD 23)
else()
    set(CMAKE_CXX_STANDARD 26)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD} (compiler ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION})")

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
# Common compiler flags
# ============================================================
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES -static-libgcc -static-libstdc++")

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lpthread -Wl,--no-whole-archive")
endif()

# Validation layers are only available in Debug builds on Linux
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR WIN32)
    add_compile_definitions(DISABLE_VALIDATION_LAYERS)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Compiling in Release mode")
    add_compile_definitions(NDEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
    if(UNIX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=array-bounds")
    endif()
endif()

# ============================================================
# MinGW Debug workaround
# ============================================================
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        string(APPEND CMAKE_CXX_FLAGS_DEBUG " -O1")
        message(STATUS "MinGW Debug: enabling -O1 to avoid type_info linker conflict")
    endif()
endif()

# ============================================================
# ---------------- COMMON LIBRARY ------------------------------
# Shared code between client and server
# ============================================================
file(GLOB_RECURSE COMMON_SOURCES
    src/common/*.cpp
)

# The common library only exists when src/common has sources
set(COMMON_LIB "")
if(COMMON_SOURCES)
    add_library(game_common STATIC ${COMMON_SOURCES})
    target_include_directories(game_common PUBLIC src/common)
    set(COMMON_LIB game_common)
else()
    message(STATUS "src/common has no sources: skipping game_common")
endif()

# ============================================================
# ---------------- SERVER EXECUTABLE ---------------------------
# Headless, no graphics, container-friendly
# ============================================================
file(GLOB_RECURSE SERVER_SOURCES
    src/server/*.cpp
)

# The server only exists when src/server has sources
if(SERVER_SOURCES)
    add_executable(${PROJECT_NAME}_server ${SERVER_SOURCES})

    target_link_libraries(${PROJECT_NAME}_server
        ${COMMON_LIB}
    )

    if(UNIX)
        target_link_libraries(${PROJECT_NAME}_server
            pthread
            dl
        )
    endif()

    set_target_properties(${PROJECT_NAME}_server PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_server")
else()
    message(STATUS "src/server has no sources: skipping ${PROJECT_NAME}_server")
endif()

# ============================================================
# ---------------- CLIENT EXECUTABLE ---------------------------
# Graphics, input, UI, shaders
# ============================================================

# ============================================================
# Vulkan
# ============================================================
if(UNIX)
    find_package(Vulkan REQUIRED)
elseif(WIN32)
    add_subdirectory(lib/Vulkan-Headers)
    if(NOT DEFINED VulkanHeaders_VERSION)
        file(STRINGS ${CMAKE_SOURCE_DIR}/lib/Vulkan-Headers/include/vulkan/vulkan_core.h
             VK_HEADER_LINE REGEX "^#define VK_HEADER_VERSION[ \t]+[0-9]+")
        string(REGEX MATCH "[0-9]+" VK_HEADER_PATCH "${VK_HEADER_LINE}")
        set(VulkanHeaders_VERSION "1.4.${VK_HEADER_PATCH}")
        message(STATUS "VulkanHeaders_VERSION: ${VulkanHeaders_VERSION}")
    endif()
    add_subdirectory(lib/Vulkan-Loader)
endif()

# ============================================================
# GLFW
# ============================================================
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

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
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

# ============================================================
# GLM
# ============================================================
if(WIN32)
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE lib/glm/glm)
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

set(ASSIMP_PATH "${CMAKE_SOURCE_DIR}/lib/assimp")

target_compile_options(
    assimp
    PRIVATE
    -Wno-error=maybe-uninitialized
)

# ============================================================
# KTX (Khronos Texture)
# ============================================================
option(KTX_FEATURE_STATIC_LIBRARY ON)
option(KTX_FEATURE_TESTS OFF)
option(KTX_FEATURE_TOOLS OFF)
option(KTX_FEATURE_VK_UPLOAD ON)

add_subdirectory(lib/KTX-Software)

# ============================================================
# Shader compilation (CLIENT ONLY)
# ============================================================
find_program(GLSLANG_VALIDATOR glslangValidator)
if(NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found")
endif()
message(STATUS "glslangValidator found: ${GLSLANG_VALIDATOR}")

set(SHADER_DIR ${CMAKE_SOURCE_DIR}/src/client/assets/shaders)
set(COMPILED_SHADER_DIR ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${COMPILED_SHADER_DIR})

set(SHADERS
    deferred_lighting.frag.glsl
    geometry_mesh_blend.frag.glsl
    particle.frag.glsl
    triangle.vert.glsl
    composite.frag.glsl
    deferred_lighting.vert.glsl
    geometry_mesh_mask.frag.glsl
    particle.vert.glsl
    composite.vert.glsl
    geometry_mesh.vert.glsl
    geometry_mesh_opaque.frag.glsl
    triangle.frag.glsl
)

set(SHADER_OUTPUTS "")
foreach(SHADER_NAME IN LISTS SHADERS)
    set(SHADER ${SHADER_DIR}/${SHADER_NAME})
    set(SPV ${COMPILED_SHADER_DIR}/${SHADER_NAME}.spv)

    # Stage is deduced from the file name: vertex.glsl / *.vert.glsl, fragment.glsl / *.frag.glsl
    if(SHADER_NAME MATCHES "\\.vert\\.glsl$")
        set(STAGE vert)
    elseif(SHADER_NAME MATCHES "\\.frag\\.glsl$")
        set(STAGE frag)
    else()
        message(FATAL_ERROR "Unknown shader stage for ${SHADER_NAME}")
    endif()

    add_custom_command(
        OUTPUT ${SPV}
        COMMAND ${GLSLANG_VALIDATOR} -V --target-env vulkan1.2 -S ${STAGE} ${SHADER} -o ${SPV}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER_NAME} to SPIR-V"
        VERBATIM
    )

    list(APPEND SHADER_OUTPUTS ${SPV})
endforeach()

add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})

# ============================================================
# Copy models (CLIENT ONLY)
# ============================================================
set(MODELS_SRC_DIR ${CMAKE_SOURCE_DIR}/src/client/assets/models)
set(MODELS_DST_DIR ${CMAKE_BINARY_DIR}/models)

add_custom_target(Models ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MODELS_DST_DIR}
)
# Only copy when the project actually has a models folder
if(EXISTS ${MODELS_SRC_DIR})
    add_custom_command(
        TARGET Models
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${MODELS_SRC_DIR} ${MODELS_DST_DIR}
    )
endif()

# ============================================================
# Client sources & executable
# ============================================================
file(GLOB_RECURSE CLIENT_SOURCES
    src/client/*.cpp
)
if(NOT CLIENT_SOURCES)
    message(FATAL_ERROR "No sources found in src/client (put main.cpp in src/client)")
endif()

add_executable(${PROJECT_NAME}_client
    ${CLIENT_SOURCES}
    ${IMGUI_SOURCES}
    ${STB_SOURCES}
)

add_dependencies(${PROJECT_NAME}_client Shaders Models)

# ============================================================
# Client include directories
# ============================================================
target_include_directories(
    ${PROJECT_NAME}_client
    PRIVATE
    src/client
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${STB_DIR}
    ${ASSIMP_PATH}
)

if(WIN32)
    target_include_directories(
        ${PROJECT_NAME}_client
        PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/Vulkan-Headers/include
    )
endif()

# ============================================================
# Client compile definitions
# ============================================================
target_compile_definitions(${PROJECT_NAME}_client PRIVATE
    VK_PROTOTYPES
)

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
    target_link_libraries(${PROJECT_NAME}_client
        ${COMMON_LIB}
        glfw
        assimp
        vulkan
        glm
        ktx
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME}_client
        ${COMMON_LIB}
        glfw
        Vulkan::Vulkan
        glm::glm
        assimp
        ktx
        pthread
        dl
        X11
        Xrandr
        Xi
        Xcursor
    )

    # Shared libs (e.g. libktx) are looked up in lib/ next to the executable
    set_target_properties(${PROJECT_NAME}_client PROPERTIES
        INSTALL_RPATH "$ORIGIN/lib"
        BUILD_WITH_INSTALL_RPATH TRUE
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