if(BUILD_RENDERER)
    add_executable(he_inc_test he_inc_test.cpp)
    target_link_libraries(he_inc_test HiveEngine)

    add_executable(procedural_noise_test procedural_noise_test.cpp)
    target_link_libraries(procedural_noise_test HiveEngine)

    add_executable(kinetic_missile_test kinetic_missile_test.cpp)
    target_link_libraries(kinetic_missile_test HiveEngine)

    add_executable(system_test system_test.cpp)
    target_link_libraries(system_test HiveEngine)
endif(BUILD_RENDERER)
