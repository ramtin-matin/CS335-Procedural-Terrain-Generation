cmake_minimum_required(VERSION 3.16)

project(CS335ProceduralTerrain LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)

add_executable(procedural_terrain
    src/Camera.cpp
    src/main.cpp
    src/MarchingCubes.cpp
    src/Shader.cpp
    src/Terrain.cpp
)

target_include_directories(procedural_terrain PRIVATE include)
target_compile_definitions(procedural_terrain PRIVATE SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

if(MSVC)
    target_compile_options(procedural_terrain PRIVATE /W4 /permissive-)
else()
    target_compile_options(procedural_terrain PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui.cpp")
    target_sources(procedural_terrain PRIVATE
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(procedural_terrain PRIVATE
        external/imgui
        external/imgui/backends
    )
else()
    message(FATAL_ERROR
        "Dear ImGui was not found.\n"
        "Recommended setup: place Dear ImGui in external/imgui.\n"
        "See external/README.md for the expected folder layout."
    )
endif()

# Prefer simple local folders in external/ so every teammate can build the same way.
set(USE_LOCAL_GLFW FALSE)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/CMakeLists.txt")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/glfw)
    set(USE_LOCAL_GLFW TRUE)
endif()

if(USE_LOCAL_GLFW)
    target_link_libraries(procedural_terrain PRIVATE glfw)
else()
    find_package(glfw3 CONFIG QUIET)

    if(TARGET glfw)
        target_link_libraries(procedural_terrain PRIVATE glfw)
    elseif(TARGET glfw3::glfw)
        target_link_libraries(procedural_terrain PRIVATE glfw3::glfw)
    else()
        message(FATAL_ERROR
            "GLFW was not found.\n"
            "Recommended setup: place the GLFW source in external/glfw.\n"
            "See external/README.md for the expected folder layout."
        )
    endif()
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glm/glm/glm.hpp")
    target_include_directories(procedural_terrain PRIVATE external/glm)
else()
    find_package(glm CONFIG QUIET)

    if(TARGET glm::glm)
        target_link_libraries(procedural_terrain PRIVATE glm::glm)
    else()
        message(FATAL_ERROR
            "GLM was not found.\n"
            "Recommended setup: place GLM in external/glm.\n"
            "See external/README.md for the expected folder layout."
        )
    endif()
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/gl.c"
   AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include/glad/gl.h")
    target_sources(procedural_terrain PRIVATE external/glad/src/gl.c)
    target_include_directories(procedural_terrain PRIVATE external/glad/include)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/glad.c"
   AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include/glad/glad.h")
    target_sources(procedural_terrain PRIVATE external/glad/src/glad.c)
    target_include_directories(procedural_terrain PRIVATE external/glad/include)
else()
    find_path(GLAD_INCLUDE_DIR NAMES glad/gl.h glad/glad.h)
    find_library(GLAD_LIBRARY NAMES glad)

    if(GLAD_INCLUDE_DIR AND GLAD_LIBRARY)
        target_include_directories(procedural_terrain PRIVATE "${GLAD_INCLUDE_DIR}")
        target_link_libraries(procedural_terrain PRIVATE "${GLAD_LIBRARY}")
    else()
        message(FATAL_ERROR
            "GLAD was not found.\n"
            "Recommended setup: place generated GLAD files in external/glad.\n"
            "See external/README.md for the expected folder layout."
        )
    endif()
endif()

target_link_libraries(procedural_terrain PRIVATE OpenGL::GL)

set_target_properties(procedural_terrain PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:procedural_terrain>"
)
