cmake_minimum_required(VERSION 3.14)

project(jackfruit-huffman-compressor VERSION 0.1.0 LANGUAGES CXX)

# Standard C++17 requirement
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directory structure
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Main executable
add_executable(jackfruit src/main.cpp)

# Testing configuration
include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Unit test executable
    add_executable(jackfruit_tests tests/unit/test_placeholder.cpp)
    target_link_libraries(jackfruit_tests PRIVATE GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(jackfruit_tests)
endif()
