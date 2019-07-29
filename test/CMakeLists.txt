if(BUILD_RENDERER)
    add_executable(he_inc_test he_inc_test.cpp)
    target_link_libraries(he_inc_test HiveEngine)

    add_executable(system_test system_test.cpp)
    target_link_libraries(system_test HiveEngine)
endif(BUILD_RENDERER)
