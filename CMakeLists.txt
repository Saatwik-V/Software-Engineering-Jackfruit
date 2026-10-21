cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(jackfruit-huffman-compressor VERSION 0.1.0 LANGUAGES CXX)

# Standard C++17 requirement
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Include directory structure
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Core library containing modular compression, io, archive, integrity components
add_library(jackfruit_core STATIC
    src/io/file_validator.cpp
    src/io/bit_reader.cpp
    src/io/bit_writer.cpp
    src/compression/frequency_table.cpp
    src/compression/huffman_tree.cpp
    src/compression/encoder.cpp
    src/compression/compressor.cpp
    src/archive/archive.cpp
    src/security/crc32.cpp
    src/archive/archive_validation.cpp
    src/archive/archive_writer.cpp
    src/decompression/decoder.cpp
)
target_include_directories(jackfruit_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Main executable (properly linking jackfruit_core)
add_executable(jackfruit src/main.cpp)
target_link_libraries(jackfruit PRIVATE jackfruit_core)

# Testing configuration
include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
    )
    # Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Disable strict ANSI mode for GoogleTest on MinGW to expose Windows POSIX symbols
    if(MINGW)
        target_compile_options(gtest PRIVATE -U__STRICT_ANSI__)
        target_compile_options(gtest_main PRIVATE -U__STRICT_ANSI__)
    endif()

    # Unit test executable
    add_executable(jackfruit_tests
        tests/unit/test_placeholder.cpp
        tests/unit/test_file_validator.cpp
        tests/unit/test_frequency_table.cpp
        tests/unit/test_huffman_tree.cpp
        tests/unit/test_bit_reader.cpp
        tests/unit/test_bit_writer.cpp
        tests/unit/test_encoder.cpp
        tests/unit/test_compressor.cpp
        tests/unit/test_archive_io.cpp
        tests/unit/test_crc32.cpp
        tests/unit/test_archive_writer.cpp
    )
    target_link_libraries(jackfruit_tests PRIVATE jackfruit_core GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(jackfruit_tests)
endif()