cmake_minimum_required(VERSION 3.20)
project(panorama LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_CLIENT "Build Panorama-client application" ON)
option(BUILD_TESTS "Build unit tests" ON)

if(WIN32)
    set(QT6_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/client/third_party/qt6-prebuilt/windows-x64")
elseif(APPLE)
    set(QT6_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/client/third_party/qt6-prebuilt/macos-arm64")
else()
    set(QT6_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/client/third_party/qt6-prebuilt/linux-x64")
endif()

set(CMAKE_PREFIX_PATH ${QT6_PREBUILT_PATH})

# Dependencies
find_package(Qt6 REQUIRED COMPONENTS Widgets)

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

if(WIN32)
    add_custom_command(TARGET panorama-client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt6::Widgets>
        $<TARGET_FILE:Qt6::Core>
        $<TARGET_FILE:Qt6::Gui>
        $<TARGET_FILE_DIR:panorama-client>
    )
endif()
