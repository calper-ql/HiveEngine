if(BUILD_RENDERER)
    add_custom_target(copy-optix-bin-folder ALL
            COMMAND cmake -E copy_directory ${OptiX_INSTALL_DIR}/bin64 ${CMAKE_BINARY_DIR}/test)

    add_executable(he_inc_test he_inc_test.cpp)
    target_link_libraries(he_inc_test HiveEngine)
    add_dependencies(he_inc_test copy-optix-bin-folder)

    add_executable(procedural_noise_test procedural_noise_test.cpp)
    target_link_libraries(procedural_noise_test HiveEngine)
    add_dependencies(procedural_noise_test copy-optix-bin-folder)

    add_executable(kinetic_missile_test kinetic_missile_test.cpp)
    target_link_libraries(kinetic_missile_test HiveEngine)
    add_dependencies(kinetic_missile_test copy-optix-bin-folder)

    add_executable(system_test system_test.cpp)
    target_link_libraries(system_test HiveEngine)
    add_dependencies(system_test copy-optix-bin-folder)


endif(BUILD_RENDERER)
