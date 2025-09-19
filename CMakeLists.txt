cmake_minimum_required(VERSION 3.20)
project(panorama LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_CLIENT "Build Qt client application" ON)
option(BUILD_TESTS "Build unit tests" ON)

# Dependencies
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Protobuf REQUIRED)

# add_subdirectory(common)

set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTOUIC ON)   # for .ui files
# set(CMAKE_AUTORCC ON)   # for .qrc resources

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
