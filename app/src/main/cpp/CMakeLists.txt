# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("connect4")

# Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(SRC_FILES
        algorithm.cpp
        board.hpp
        hash_table.hpp
        )

add_library(connect4 SHARED ${SRC_FILES})

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       connect4

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

target_include_directories(connect4 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Testing configuration
option(BUILD_TESTS "Build tests" ON)

IF(BUILD_TESTS)
    message("Testing enabled")
    set(TEST_SRC_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/test.cpp
            )
    add_subdirectory(../../../gtest)
    enable_testing()
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    add_executable(runUnitTests ${TEST_SRC_FILES})
    target_link_libraries(runUnitTests gtest gtest_main)
    add_test(UnitTests runUnitTests)
    target_compile_options(connect4 PRIVATE -g)
ENDIF()