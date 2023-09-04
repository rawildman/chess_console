set(LIB_SRC 
    chess.cpp 
    board.cpp 
    board_positions.cpp
    board_view.cpp 
    game.cpp 
    load_save.cpp 
    logic.cpp 
    user_interface.cpp 
    validation.cpp)
set(LIB_HDR 
    chess.hpp 
    board.hpp 
    board_positions.hpp
    board_view.hpp 
    game.hpp 
    load_save.hpp 
    logic.hpp 
    pieces.hpp
    user_interface.hpp 
    validation.hpp)

add_library(core ${LIB_SRC} ${LIB_HDR})
target_include_directories(core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
set_property(TARGET core PROPERTY CXX_STANDARD 20)
set_property(TARGET core PROPERTY CXX_STANDARD_REQUIRED ON)

if(${BUILD_UNIT_TESTS})
    add_executable(core_unittests ${LIB_SRC})
    set_property(TARGET core_unittests PROPERTY CXX_STANDARD 20)
    target_compile_definitions(core_unittests PUBLIC UNIT_TEST=1)
    target_compile_options(core_unittests PRIVATE -fprofile-arcs -ftest-coverage)
    target_link_libraries(core_unittests PRIVATE Catch2::Catch2WithMain -lgcov)

    catch_discover_tests(core_unittests)
endif()
