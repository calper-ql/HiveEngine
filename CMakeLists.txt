cmake_minimum_required(VERSION 3.14)
project(HiveEngine VERSION 1.0.0 LANGUAGES CXX)

option(BUILD_RENDERER "Build the renderer for HiveEngine" OFF)

if(BUILD_RENDERER)
    set(Vulkan_INCLUDE_DIR ~/SDK/1.1.106.0/x86_64)
    find_package(Vulkan REQUIRED)

    message(STATUS "==== VULKAN_FOUND: " ${Vulkan_FOUND})
    message(STATUS "Vulkan_INCLUDE_DIRS: " ${Vulkan_INCLUDE_DIRS})
    message(STATUS "Vulkan_LIBRARIES:    " ${Vulkan_LIBRARIES})

    include_directories(${Vulkan_INCLUDE_DIRS}/include)
    find_package(glfw3 REQUIRED)
endif(BUILD_RENDERER)

find_package(glm REQUIRED)

set(SOURCES src/HiveEngine.cpp include/HiveEngine/LODSquare.h src/LODSquare.cpp include/HiveEngine/DynamicSphere.h include/HiveEngine/RadialGenerator.h src/RadialGenerator.cpp include/HiveEngine/Texture.h src/DynamicSphere.cpp include/HiveEngine/Entity.h src/Entity.cpp include/HiveEngine/Utilities.h src/Utilities.cpp include/HiveEngine/Common.h src/Common.cpp include/HiveEngine/Mesh.h)

if(BUILD_RENDERER)
    set(SOURCES ${SOURCES} include/HiveEngine/Renderer/Context.h src/Renderer/Context.cpp)
endif(BUILD_RENDERER)

message(STATUS "Sources:    " ${SOURCES})
add_library(HiveEngine SHARED ${SOURCES})

target_include_directories(HiveEngine
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
target_compile_features(HiveEngine PUBLIC cxx_std_17)

target_link_libraries(HiveEngine PUBLIC glm)
if(BUILD_RENDERER)
    target_link_libraries(HiveEngine PUBLIC glfw)
    target_link_libraries(HiveEngine PUBLIC ${Vulkan_LIBRARIES})
endif(BUILD_RENDERER)

include(GNUInstallDirs)

message(STATUS "CMAKE_INSTALL_INCLUDEDIR:" ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS HiveEngine
        EXPORT HiveEngine-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/HiveEngine)

install(EXPORT
            HiveEngine-targets
        FILE
            HiveEngineTargets.cmake
        NAMESPACE
            HiveEngine::
        DESTINATION
            ${INSTALL_CONFIGDIR}
        )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/HiveEngineConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/HiveEngineConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/HiveEngineConfig.cmake
        INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
        )

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/HiveEngineConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/HiveEngineConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR}
        )


configure_file(src/HiveEngine.pc.in src/HiveEngine.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/src/HiveEngine.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

export(EXPORT HiveEngine-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/HiveEngineTargets.cmake NAMESPACE HiveEngine::)

export(PACKAGE HiveEngine)
add_subdirectory(test)

