cmake_minimum_required(VERSION 3.10)
project(HiveEngine VERSION 1.0.0 LANGUAGES CXX C)

option(BUILD_RENDERER "Build the renderer for HiveEngine" OFF)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(BUILD_RENDERER)
    add_subdirectory(third_party/freetype-2.10.0)

    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    message(STATUS "OPENGL INCLUDE:" ${OpenGL_INCLUDE_DIRS})

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/glfw-3.3)

    find_package(CUDA 10.1 REQUIRED)
    find_package(OptiX REQUIRED)
    include_directories(${OptiX_INCLUDE})
    include_directories(${CUDA_INCLUDE_DIRS})


    set(CUDA_NVCC_FLAGS "-arch sm_30 --keep-device-functions")

endif(BUILD_RENDERER)

set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glm)
add_subdirectory(third_party/spdlog-1.3.1)
add_subdirectory(third_party/assimp-4.1.0)
set(ASSIMP_INCLUDE_DIR third_party/assimp-4.1.0/include ${CMAKE_BINARY_DIR}/third_party/assimp-4.1.0/include)

set(SOURCES src/HiveEngine.cpp include/HiveEngine/LODSquare.h src/LODSquare.cpp include/HiveEngine/DynamicSphere.h include/HiveEngine/RadialGenerator.h src/RadialGenerator.cpp include/HiveEngine/Texture.h src/DynamicSphere.cpp include/HiveEngine/PhysicalEntity.h src/PhysicalEntity.cpp include/HiveEngine/Utilities.h src/Utilities.cpp include/HiveEngine/Common.h src/Common.cpp include/HiveEngine/Mesh.h include/HiveEngine/Buffer.hpp include/HiveEngine/stb/stb_image.h include/HiveEngine/Texture.h src/Texture.cpp include/HiveEngine/noise.h src/noise.cpp src/Context.cpp include/HiveEngine/Context.h)

if(BUILD_RENDERER)
    set(SOURCES ${SOURCES} include/HiveEngine/Renderer/Context.h src/Renderer/Context.cpp include/HiveEngine/Renderer/Camera.h src/Renderer/Camera.cpp include/HiveEngine/Renderer/glad/glad.h include/HiveEngine/Renderer/KHR/khrplatform.h include/HiveEngine/Renderer/Drawing.h src/Renderer/Drawing.cpp include/HiveEngine/Renderer/FontManager.h src/Renderer/FontManager.cpp include/HiveEngine/Renderer/GlyphDrawing.h src/Renderer/GlyphDrawing.cpp include/HiveEngine/Renderer/TextDrawing.h src/Renderer/TextDrawing.cpp src/Renderer/LineDrawing.cpp include/HiveEngine/Renderer/LineDrawing.h src/Renderer/OptixDrawing.cpp include/HiveEngine/Renderer/OptixDrawing.h src/Context.cpp include/HiveEngine/Context.h src/Renderer/AABBDrawing.cpp include/HiveEngine/Renderer/AABBDrawing.h src/Renderer/EntityDrawing.cpp include/HiveEngine/Renderer/EntityDrawing.h)
    file(GLOB_RECURSE GLSL_SOURCE_FILES
            "shaders/*.frag"
            "shaders/*.vert"
            )
    message(STATUS "GLSL programs: " ${GLSL_SOURCE_FILES})

    add_custom_target(copy-shader-folder ALL
            COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders)

    file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/shaders/optix/*.cu")
    foreach(file ${files})
        CUDA_COMPILE_PTX(i ${file})
        set(RTX_SHADERS ${i} ${RTX_SHADERS} shaders/optix/rt_function.h shaders/optix/payloads.h)
    endforeach()

    message(STATUS "RTX SHADERS: " ${RTX_SHADERS})
endif(BUILD_RENDERER)

include_directories(include)
include_directories(${ASSIMP_INCLUDE_DIR})
message(STATUS "Assimp: " ${ASSIMP_INCLUDE_DIR})

message(STATUS "Sources:    " ${SOURCES})
add_library(HiveEngine STATIC ${SOURCES} src/Renderer/glad.c ${RTX_SHADERS})
target_compile_features(HiveEngine PUBLIC cxx_std_17)

target_link_libraries(HiveEngine PUBLIC assimp)
target_link_libraries(HiveEngine PUBLIC glm)
target_link_libraries(HiveEngine PUBLIC spdlog)
IF(WIN32)

ELSE()
    target_link_libraries(HiveEngine PUBLIC stdc++fs)
ENDIF()

add_custom_target(copy-data-folder ALL
        COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)

if(BUILD_RENDERER)
    target_link_libraries(HiveEngine PUBLIC freetype)
    target_link_libraries(HiveEngine PUBLIC glfw ${GLFW_LIBRARIES})
    target_link_libraries(HiveEngine PUBLIC ${OPENGL_gl_LIBRARY})
    target_link_libraries(HiveEngine PUBLIC optix)

    add_dependencies(HiveEngine copy-shader-folder)
endif(BUILD_RENDERER)

add_dependencies(HiveEngine copy-data-folder)

add_subdirectory(test)

