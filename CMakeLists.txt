cmake_minimum_required(VERSION 3.20)
project(panorama LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_CLIENT "Build Panorama-client application" ON)
option(BUILD_TESTS "Build unit tests" ON)

# add_subdirectory(common)

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

