cmake_minimum_required(VERSION 3.10)
project(HiveEngine VERSION 1.0.0 LANGUAGES CXX)

option(BUILD_RENDERER "Build the renderer for HiveEngine" OFF)

if(BUILD_RENDERER)
    set(Vulkan_INCLUDE_DIR /home/calper//SDK/Vulkan-1.1.106.0/x86_64)
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
    add_subdirectory(glfw-3.3)
endif(BUILD_RENDERER)

set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(glm)

set(SOURCES src/HiveEngine.cpp include/HiveEngine/LODSquare.h src/LODSquare.cpp include/HiveEngine/DynamicSphere.h include/HiveEngine/RadialGenerator.h src/RadialGenerator.cpp include/HiveEngine/Texture.h src/DynamicSphere.cpp include/HiveEngine/Entity.h src/Entity.cpp include/HiveEngine/Utilities.h src/Utilities.cpp include/HiveEngine/Common.h src/Common.cpp include/HiveEngine/Mesh.h)

if(BUILD_RENDERER)
    set(SOURCES ${SOURCES} include/HiveEngine/Renderer/Context.h src/Renderer/Context.cpp include/HiveEngine/Renderer/Directive.h src/Renderer/Directive.cpp include/HiveEngine/Renderer/TestDirective.h src/Renderer/TestDirective.cpp include/HiveEngine/Renderer/vk_mem_alloc.h)
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

message(STATUS "Sources:    " ${SOURCES})
add_library(HiveEngine STATIC ${SOURCES})
target_compile_features(HiveEngine PUBLIC cxx_std_17)

target_link_libraries(HiveEngine PUBLIC glm)
target_link_libraries(HiveEngine PUBLIC stdc++fs)

if(BUILD_RENDERER)
    target_link_libraries(HiveEngine PUBLIC glfw)
    target_link_libraries(HiveEngine PUBLIC ${Vulkan_LIBRARIES})
    add_dependencies(HiveEngine Shaders)
endif(BUILD_RENDERER)

add_subdirectory(test)

