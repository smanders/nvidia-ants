cmake_minimum_required(VERSION 3.15)
project(AntGridAPI VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Add include directory to include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create library from the API
add_library(ant_grid_api STATIC
    src/ant_grid_api.cpp
)

target_include_directories(ant_grid_api PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Example executable
add_executable(ant_grid_example
    test/example_usage.cpp
)

target_link_libraries(ant_grid_example
    ant_grid_api
)

# Test executable
add_executable(ant_grid_test
    test/ant_grid_test.cpp
)

target_link_libraries(ant_grid_test
    ant_grid_api
)

# Installation
install(TARGETS ant_grid_api ant_grid_example ant_grid_test
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES include/ant_grid_api.hpp
    DESTINATION include
)

# Custom target to run tests
add_custom_target(run_tests
    COMMAND ant_grid_test
    DEPENDS ant_grid_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running ant grid API tests"
)

# Custom target to run example
add_custom_target(run_example
    COMMAND ant_grid_example
    DEPENDS ant_grid_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running ant grid API example"
)
