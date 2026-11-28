cmake_minimum_required(VERSION 3.12)

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
# Build options
# ============================================================
# The server is headless: building it must not require Vulkan, GLFW, KTX,
# X11 or glslangValidator. Turn BUILD_CLIENT off for server-only builds
# (containers, CI).
option(BUILD_CLIENT "Build the graphical client" ON)
option(BUILD_SERVER "Build the headless server" ON)

if(NOT BUILD_CLIENT AND NOT BUILD_SERVER)
    message(FATAL_ERROR "Nothing to build: enable BUILD_CLIENT and/or BUILD_SERVER")
endif()

# Branching factor of the BVH. Single source of truth: it is passed to the C++
# code (as a compile definition) and to the GLSL shaders (as a -D define), so
# the CPU layout and the shader layout cannot drift apart.
set(BVH_WIDTH 8 CACHE STRING "BVH branching factor shared by C++ and GLSL")

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
# ---------------- DEPENDENCIES USED BY COMMON -----------------
# Everything here is needed by both client and server.
# Graphics-only dependencies live in the client section below.
# ============================================================

# ------------------------------------------------------------
# GLM
# ------------------------------------------------------------
if(WIN32)
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE lib/glm/glm)
    set(GLM_TARGET glm)
else()
    find_package(glm REQUIRED)
    set(GLM_TARGET glm::glm)
endif()

# ------------------------------------------------------------
# Assimp (the only model importer, used from src/common/assets)
# ------------------------------------------------------------
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
# ---------------- COMMON LIBRARY ------------------------------
# Shared code between client and server. It must never depend on
# Vulkan, windowing, UI or the GPU.
# ============================================================
# CONFIGURE_DEPENDS re-runs the glob when files are added, moved or removed,
# which matters a lot while the source tree is being reorganized.
file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS
    src/common/*.cpp
)

# Header-only fallback: templated code (e.g. BVH builders) may live entirely
# in headers, so the library becomes an INTERFACE target when there are no
# .cpp files yet.
if(COMMON_SOURCES)
    add_library(game_common STATIC ${COMMON_SOURCES})
    set(COMMON_SCOPE PUBLIC)
else()
    add_library(game_common INTERFACE)
    set(COMMON_SCOPE INTERFACE)
    message(STATUS "src/common has no .cpp files: game_common is header-only")
endif()

# The include root is "src", so includes read as <common/accel/...> and cannot
# collide with same-named headers in client/ or server/.
target_include_directories(game_common ${COMMON_SCOPE}
    ${CMAKE_SOURCE_DIR}/src
    ${ASSIMP_PATH}
)

target_compile_definitions(game_common ${COMMON_SCOPE}
    BVH_WIDTH=${BVH_WIDTH}
)

target_link_libraries(game_common ${COMMON_SCOPE}
    ${GLM_TARGET}
    assimp
)

# ============================================================
# ---------------- SERVER EXECUTABLE ---------------------------
# Headless, no graphics, container-friendly
# ============================================================
if(BUILD_SERVER)
    file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
        src/server/*.cpp
    )

    if(SERVER_SOURCES)
        add_executable(${PROJECT_NAME}_server ${SERVER_SOURCES})

        target_include_directories(${PROJECT_NAME}_server PRIVATE
            src/server
        )

        target_link_libraries(${PROJECT_NAME}_server
            game_common
        )

        if(UNIX)
            target_link_libraries(${PROJECT_NAME}_server
                pthread
                dl
            )
        endif()
    else()
        message(STATUS "src/server has no sources: skipping ${PROJECT_NAME}_server")
    endif()
endif()

# ============================================================
# ---------------- CLIENT EXECUTABLE ---------------------------
# Graphics, input, UI, shaders
# ============================================================
if(BUILD_CLIENT)

    # --------------------------------------------------------
    # Vulkan
    # --------------------------------------------------------
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

    # --------------------------------------------------------
    # GLFW
    # --------------------------------------------------------
    option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
    option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
    option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
    option(GLFW_INSTALL "Generate installation target" OFF)
    option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

    add_subdirectory(lib/glfw)

    # --------------------------------------------------------
    # Dear ImGui
    # --------------------------------------------------------
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

    # --------------------------------------------------------
    # stb_image
    # --------------------------------------------------------
    # Only the include directory is needed here: the stb implementation file
    # (stb_image_impl.cpp) is picked up by the client source glob wherever it
    # lives, so its path is not hardcoded anymore.
    set(STB_DIR lib/stb)

    # --------------------------------------------------------
    # KTX (Khronos Texture)
    # --------------------------------------------------------
    option(KTX_FEATURE_STATIC_LIBRARY ON)
    option(KTX_FEATURE_TESTS OFF)
    option(KTX_FEATURE_TOOLS OFF)
    option(KTX_FEATURE_VK_UPLOAD ON)

    add_subdirectory(lib/KTX-Software)

    # --------------------------------------------------------
    # Shader compilation
    # --------------------------------------------------------
    find_program(GLSLANG_VALIDATOR glslangValidator)
    if(NOT GLSLANG_VALIDATOR)
        message(FATAL_ERROR "glslangValidator not found")
    endif()
    message(STATUS "glslangValidator found: ${GLSLANG_VALIDATOR}")

    set(SHADER_DIR ${CMAKE_SOURCE_DIR}/src/client/shaders)
    set(COMPILED_SHADER_DIR ${CMAKE_BINARY_DIR}/shaders)
    file(MAKE_DIRECTORY ${COMPILED_SHADER_DIR})

    # Shaders are discovered automatically so the folder layout (per pass,
    # raytracing/, ...) can change without touching this file.
    # Only files with a stage suffix are compiled; any other *.glsl file
    # (for example raytracing/bvh.glsl) is an include.
    file(GLOB_RECURSE SHADER_SOURCES CONFIGURE_DEPENDS
        ${SHADER_DIR}/*.vert.glsl
        ${SHADER_DIR}/*.frag.glsl
        ${SHADER_DIR}/*.comp.glsl
    )

    # Every .glsl file is a dependency of every shader, so editing an include
    # triggers a rebuild of the shaders that use it.
    file(GLOB_RECURSE SHADER_ALL_FILES CONFIGURE_DEPENDS
        ${SHADER_DIR}/*.glsl
    )

    set(SHADER_OUTPUTS "")
    set(SHADER_SEEN_NAMES "")
    foreach(SHADER IN LISTS SHADER_SOURCES)
        get_filename_component(SHADER_NAME ${SHADER} NAME)

        # Compiled shaders are written flat into build/shaders, so file names
        # must be unique across subfolders.
        if(SHADER_NAME IN_LIST SHADER_SEEN_NAMES)
            message(FATAL_ERROR "Duplicate shader file name: ${SHADER_NAME}")
        endif()
        list(APPEND SHADER_SEEN_NAMES ${SHADER_NAME})

        set(SPV ${COMPILED_SHADER_DIR}/${SHADER_NAME}.spv)

        # Stage is deduced from the file name: *.vert.glsl, *.frag.glsl, *.comp.glsl
        if(SHADER_NAME MATCHES "\\.vert\\.glsl$")
            set(STAGE vert)
        elseif(SHADER_NAME MATCHES "\\.frag\\.glsl$")
            set(STAGE frag)
        elseif(SHADER_NAME MATCHES "\\.comp\\.glsl$")
            set(STAGE comp)
        else()
            message(FATAL_ERROR "Unknown shader stage for ${SHADER_NAME}")
        endif()

        add_custom_command(
            OUTPUT ${SPV}
            COMMAND ${GLSLANG_VALIDATOR} -V --target-env vulkan1.2
                    -DBVH_WIDTH=${BVH_WIDTH}
                    -I${SHADER_DIR}
                    -S ${STAGE} ${SHADER} -o ${SPV}
            DEPENDS ${SHADER} ${SHADER_ALL_FILES}
            COMMENT "Compiling ${SHADER_NAME} to SPIR-V"
            VERBATIM
        )

        list(APPEND SHADER_OUTPUTS ${SPV})
    endforeach()

    add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})

    # --------------------------------------------------------
    # Client sources & executable
    # --------------------------------------------------------
    file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
        src/client/*.cpp
    )
    if(NOT CLIENT_SOURCES)
        message(FATAL_ERROR "No sources found in src/client (put main.cpp in src/client)")
    endif()

    add_executable(${PROJECT_NAME}_client
        ${CLIENT_SOURCES}
        ${IMGUI_SOURCES}
    )

    add_dependencies(${PROJECT_NAME}_client Shaders)

    # --------------------------------------------------------
    # Client include directories
    # --------------------------------------------------------
    target_include_directories(
        ${PROJECT_NAME}_client
        PRIVATE
        src/client
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${STB_DIR}
    )

    if(WIN32)
        target_include_directories(
            ${PROJECT_NAME}_client
            PRIVATE
            ${CMAKE_SOURCE_DIR}/lib/Vulkan-Headers/include
        )
    endif()

    # --------------------------------------------------------
    # Client compile definitions
    # --------------------------------------------------------
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

    # --------------------------------------------------------
    # Client libraries
    # --------------------------------------------------------
    # glm and assimp come transitively from game_common.
    if(WIN32)
        target_link_libraries(${PROJECT_NAME}_client
            game_common
            glfw
            vulkan
            ktx
        )
    elseif(UNIX)
        target_link_libraries(${PROJECT_NAME}_client
            game_common
            glfw
            Vulkan::Vulkan
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

endif()

# ============================================================
# Assets (namespaced and versioned: assets/<namespace>/<version>/<asset-name>)
# ============================================================
# Copied next to the executables. Both client and server read from here
# (the server needs the geometry). Assets downloaded at runtime by the
# client are written into the same tree, under their own namespace.
set(ASSETS_SRC_DIR ${CMAKE_SOURCE_DIR}/assets)
set(ASSETS_DST_DIR ${CMAKE_BINARY_DIR}/assets)

add_custom_target(Assets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ASSETS_DST_DIR}
)
# Only copy when the project actually has an assets folder
if(EXISTS ${ASSETS_SRC_DIR})
    add_custom_command(
        TARGET Assets
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SRC_DIR} ${ASSETS_DST_DIR}
    )
endif()

if(TARGET ${PROJECT_NAME}_client)
    add_dependencies(${PROJECT_NAME}_client Assets)
endif()
if(TARGET ${PROJECT_NAME}_server)
    add_dependencies(${PROJECT_NAME}_server Assets)
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