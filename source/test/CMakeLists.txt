if(${BUILD_UNIT_TESTS})
    add_executable(regression_tests regression_tests.cpp)
    set_property(TARGET regression_tests PROPERTY CXX_STANDARD 20)
    target_link_libraries(regression_tests PRIVATE chess_core Catch2::Catch2WithMain)

    FILE(COPY dat DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    catch_discover_tests(regression_tests)
endif()