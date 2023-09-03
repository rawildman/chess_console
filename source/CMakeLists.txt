#==============================================================================
#    _____ _    _ ______  _____ _____
#   / ____| |  | |  ____|/ ____/ ____|
#  | |    | |__| | |__  | (___| (___ 
#  | |    |  __  |  __|  \___ \\___ \
#  | |____| |  | | |____ ____) |___) |
#   \_____|_|  |_|______|_____/_____/
#
#  /source/CMakeLists.txt
#
#==============================================================================

cmake_minimum_required (VERSION 3.20)

project (chess CXX)

option(BUILD_UNIT_TESTS "Build unit tests" ON)

set(CHESS_SRC chess.cpp game.cpp load_save.cpp user_interface.cpp board.cpp logic.cpp validation.cpp)

add_executable(chess ${CHESS_SRC} main.cpp)

set_property(TARGET chess PROPERTY CXX_STANDARD 20)
set_property(TARGET chess PROPERTY CXX_STANDARD_REQUIRED ON) 

if(${BUILD_UNIT_TESTS})
    message(STATUS "Building unit tests")
    enable_testing()

    Include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.3.2
    )

    FetchContent_MakeAvailable(Catch2)
    
    add_executable(unittests ${CHESS_SRC})
    set_property(TARGET unittests PROPERTY CXX_STANDARD 20)
    target_compile_definitions(unittests PUBLIC UNIT_TEST=1)
    target_compile_options(unittests PRIVATE -fprofile-arcs -ftest-coverage)
    target_link_libraries(unittests PRIVATE Catch2::Catch2WithMain -lgcov)

    # Register tests
    #include(CTest)
    #include(Catch)
    #catch_discover_tests(unittests)
endif()
