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

    include(CTest)
    include(Catch)
endif()

add_subdirectory(core)

set(EXE_SRC main.cpp)
add_executable(chess ${EXE_SRC})
set_property(TARGET chess PROPERTY CXX_STANDARD 20)
set_property(TARGET chess PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(chess PRIVATE chess_core)
