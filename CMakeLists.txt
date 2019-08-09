cmake_minimum_required(VERSION 3.10)
project(HiveEngine VERSION 1.0.0 LANGUAGES CXX)

option(BUILD_RENDERER "Build the renderer for HiveEngine" ON)

if(BUILD_RENDERER)
    add_subdirectory(third_party/freetype-2.10.0)

    set(Vulkan_INCLUDE_DIR /home/calper//SDK/Vulkan-1.1.108.0/x86_64)
    IF (WIN32)
        set(Vulkan_INCLUDE_DIR C:/VulkanSDK/1.1.114.0/)
    ELseIF (APPLE)
        set(Vulkan_INCLUDE_DIR /Users/calper/Projects/vulkansdk-macos-1.1.108.0/macOS)
    ENDIF()

    find_package(Vulkan REQUIRED)
    message(STATUS "==== VULKAN_FOUND: " ${Vulkan_FOUND})
    message(STATUS "Vulkan_INCLUDE_DIRS: " ${Vulkan_INCLUDE_DIRS})
    message(STATUS "Vulkan_LIBRARIES:    " ${Vulkan_LIBRARIES})
    include_directories(${Vulkan_INCLUDE_DIRS}/include)
    IF (WIN32)
        set(GLSL_VALIDATOR ${Vulkan_INCLUDE_DIR}/bin/glslangValidator.exe)
    ELSE()
        set(GLSL_VALIDATOR ${Vulkan_INCLUDE_DIR}/bin/glslangValidator)
    ENDIF()


    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/glfw-3.3)
endif(BUILD_RENDERER)

set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glm)
add_subdirectory(third_party/spdlog-1.3.1)
add_subdirectory(third_party/assimp-4.1.0)
set(ASSIMP_INCLUDE_DIR third_party/assimp-4.1.0/include ${CMAKE_BINARY_DIR}/third_party/assimp-4.1.0/include)

set(SOURCES src/HiveEngine.cpp include/HiveEngine/LODSquare.h src/LODSquare.cpp include/HiveEngine/DynamicSphere.h include/HiveEngine/RadialGenerator.h src/RadialGenerator.cpp include/HiveEngine/Texture.h src/DynamicSphere.cpp include/HiveEngine/Utilities.h src/Utilities.cpp include/HiveEngine/Common.h src/Common.cpp include/HiveEngine/Mesh.h include/HiveEngine/Buffer.hpp include/HiveEngine/stb/stb_image.h include/HiveEngine/Texture.h src/Texture.cpp include/HiveEngine/noise.h src/noise.cpp src/Context.cpp include/HiveEngine/Context.h src/Mesh.cpp src/AssetManager.cpp include/HiveEngine/AssetManager.h)
if(BUILD_RENDERER)
    set(SOURCES ${SOURCES} include/HiveEngine/Renderer/Context.h src/Renderer/Context.cpp include/HiveEngine/Renderer/Directive.h src/Renderer/Directive.cpp include/HiveEngine/Renderer/StandardDirective.h src/Renderer/StandardDirective.cpp include/HiveEngine/Renderer/vk_mem_alloc.h include/HiveEngine/Renderer/Drawing.h src/Renderer/Drawing.cpp include/HiveEngine/Renderer/LineDrawing.h src/Renderer/LineDrawing.cpp  src/Texture.cpp include/HiveEngine/Renderer/GlyphDrawing.h src/Renderer/GlyphDrawing.cpp include/HiveEngine/Renderer/Camera.h src/Renderer/Camera.cpp include/HiveEngine/Renderer/FontManager.h src/Renderer/FontManager.cpp src/Renderer/TextDrawing.cpp include/HiveEngine/Renderer/TextDrawing.h src/Renderer/AABBDrawing.cpp include/HiveEngine/Renderer/AABBDrawing.h include/HiveEngine/Renderer/GPUBuffer.hpp include/HiveEngine/Renderer/MeshDrawing.h src/Renderer/MeshDrawing.cpp)
    file(GLOB_RECURSE GLSL_SOURCE_FILES
            "shaders/*.frag"
            "shaders/*.vert"
            )
    message(STATUS "GLSL programs: " ${GLSL_SOURCE_FILES})

    foreach(GLSL ${GLSL_SOURCE_FILES})
        get_filename_component(FILE_NAME ${GLSL} NAME)
        set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
        add_custom_command(
                OUTPUT ${SPIRV}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
                COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
                DEPENDS ${GLSL})
        list(APPEND SPIRV_BINARY_FILES ${SPIRV})
    endforeach(GLSL)

    add_custom_target(
            Shaders
            DEPENDS ${SPIRV_BINARY_FILES}
    )
endif(BUILD_RENDERER)

include_directories(include)
include_directories(${ASSIMP_INCLUDE_DIR})
message(STATUS "Assimp: " ${ASSIMP_INCLUDE_DIR})

include_directories(include)

message(STATUS "Sources:    " ${SOURCES})
add_library(HiveEngine STATIC ${SOURCES})
target_compile_features(HiveEngine PUBLIC cxx_std_17)

target_link_libraries(HiveEngine PUBLIC assimp)
target_link_libraries(HiveEngine PUBLIC glm)
target_link_libraries(HiveEngine PUBLIC spdlog)
IF(LINUX)
    target_link_libraries(HiveEngine PUBLIC stdc++fs)
ENDIF()

add_custom_target(copy-data-folder ALL
        COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data)

if(BUILD_RENDERER)
    target_link_libraries(HiveEngine PUBLIC freetype)
    target_link_libraries(HiveEngine PUBLIC glfw ${GLFW_LIBRARIES})
    target_link_libraries(HiveEngine PUBLIC ${Vulkan_LIBRARIES})
    add_dependencies(HiveEngine Shaders)
endif(BUILD_RENDERER)

add_dependencies(HiveEngine copy-data-folder)

add_subdirectory(test)

