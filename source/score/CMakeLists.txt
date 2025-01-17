set(LIB_SRC
    defends_attack.cpp
    escapes_attack.cpp
    takes_piece.cpp 
    threatens_king.cpp 
    under_attack.cpp)
set(LIB_HDR 
    core_fwds.hpp 
    defends_attack.hpp
    escapes_attack.hpp
    takes_piece.hpp 
    threatens_king.hpp 
    under_attack.hpp)

add_library(score ${LIB_SRC} ${LIB_HDR})
target_include_directories(score PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_include_directories(score PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>)
message(STATUS "Project dir: ${PROJECT_SOURCE_DIR}")
target_link_libraries(score PRIVATE core)
set_property(TARGET score PROPERTY CXX_STANDARD 20)
set_property(TARGET score PROPERTY CXX_STANDARD_REQUIRED ON)

if(${BUILD_UNIT_TESTS})
    set(TEST_SRC test_utility.cpp)
    add_executable(score_unittests ${LIB_SRC} ${TEST_SRC})
    set_property(TARGET score_unittests PROPERTY CXX_STANDARD 20)
    target_compile_definitions(score_unittests PUBLIC UNIT_TEST=1)
    target_compile_options(score_unittests PRIVATE -fprofile-arcs -ftest-coverage)
    target_include_directories(score_unittests PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>)
    target_link_libraries(score_unittests PRIVATE core Catch2::Catch2WithMain -lgcov)

    catch_discover_tests(score_unittests)
endif()
