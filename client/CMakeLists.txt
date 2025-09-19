cmake_minimum_required(VERSION 3.20)

set(CLIENT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/argparser.cpp
)

set(CLIENT_HEADERS
    include/client/mainwindow.hpp
    include/client/argparser.hpp
)

add_executable(panorama-client
    ${CLIENT_SOURCES}
    ${CLIENT_HEADERS}
)

target_include_directories(panorama-client
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(panorama-client
    PRIVATE Qt6::Widgets
)