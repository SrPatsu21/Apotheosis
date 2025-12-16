cmake_minimum_required(VERSION 3.10)

# ----------------------------
# Project name variable
# ----------------------------
set(PROJECT_NAME "ProjectName")
project(${PROJECT_NAME})

# ----------------------------
# C++ standard
# ----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Common compiler flags
# ----------------------------
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES -static-libgcc -static-libstdc++")

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lpthread -Wl,--no-whole-archive")
endif()

# ----------------------------
# Platform-specific definitions
# ----------------------------
if(WIN32)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
elseif(UNIX)
    add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
endif()

# ----------------------------
# Force Debug if not set
# ----------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# ----------------------------
# Disable validation layers in Release/Win
# ----------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR WIN32)
    add_compile_definitions(DISABLE_VALIDATION_LAYERS)
endif()

# ----------------------------
# Release-specific flags
# ----------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Compiling in Release mode")
    add_compile_definitions(NDEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
    if(UNIX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=array-bounds")
    endif()
endif()

# ----------------------------
# Vulkan
# ----------------------------
if(UNIX)
    find_package(Vulkan REQUIRED)
elseif(WIN32)
    set(VULKAN_SDK_PATH "./lib/vulkan-sdk-win")
    include_directories(${VULKAN_SDK_PATH}/Include)
    link_directories(${VULKAN_SDK_PATH}/Lib)
endif()

# ----------------------------
# GLFW
# ----------------------------
    option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
    option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
    option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
    option(GLFW_INSTALL "Generate installation target" OFF)
    option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
    add_subdirectory(lib/glfw)
    # add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
    include_directories(lib/glfw/include)

# ----------------------------
# Dear ImGui
# ----------------------------
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

# ----------------------------
# GLM
# ----------------------------
if(WIN32)
    set(GLM_PATH "${CMAKE_SOURCE_DIR}/lib/glm/glm-1.0.1")
    include_directories(${GLM_PATH})
else()
    find_package(glm REQUIRED)
endif()

# ----------------------------
# glslangValidator (shader compilation)
# ----------------------------
find_program(GLSLANG_VALIDATOR glslangValidator
    PATHS
        ${VULKAN_SDK_PATH}/Bin
        /usr/bin
        /usr/local/bin
)
if(NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found!")
endif()
message(STATUS "glslangValidator found: ${GLSLANG_VALIDATOR}")

# ----------------------------
# Shader compilation
# ----------------------------
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
    set(SPV_FILE ${COMPILED_SHADER_DIR}/${SHADER_NAME}.spv)

    if(SHADER_NAME MATCHES "vertex.glsl")
        set(STAGE "vert")
    elseif(SHADER_NAME MATCHES "fragment.glsl")
        set(STAGE "frag")
    else()
        message(FATAL_ERROR "Unknown shader stage for ${SHADER_NAME}")
    endif()

    add_custom_command(
        OUTPUT ${SPV_FILE}
        COMMAND ${GLSLANG_VALIDATOR} -V -S ${STAGE} ${SHADER} -o ${SPV_FILE}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER_NAME} to SPIR-V"
        VERBATIM
    )

    list(APPEND SHADER_OUTPUTS ${SPV_FILE})
endforeach()
add_custom_target(Shaders ALL DEPENDS ${SHADER_OUTPUTS})

# ----------------------------
# stb_image
# ----------------------------
set(STB_DIR ${CMAKE_SOURCE_DIR}/lib/stb)
include_directories(${STB_DIR})
set(STB_SOURCES src/client/image/stb_image_impl.cpp) # adicione outros headers conforme necessário

# ----------------------------
# Assimp
# ----------------------------
add_subdirectory(lib/assimp)
include_directories(lib/assimp/include)

# ----------------------------
# Sources and executable
# ----------------------------
file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} ${STB_SOURCES})
add_dependencies(${PROJECT_NAME} Shaders)

# ----------------------------
# Platform-specific target properties
# ----------------------------
if(WIN32)
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
            OUTPUT_NAME "${PROJECT_NAME}"
            SUFFIX ".exe"
    )
elseif(UNIX)
    set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}"
        SUFFIX ""
        INSTALL_RPATH "$ORIGIN/lib/linux"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

# ----------------------------
# Libraries
# ----------------------------
if(WIN32)
    set(LIBRARIES glfw assimp vulkan-1)
elseif(UNIX)
    set(LIBRARIES glfw Vulkan::Vulkan pthread dl X11 Xrandr Xi Xcursor glm::glm assimp)
endif()

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})