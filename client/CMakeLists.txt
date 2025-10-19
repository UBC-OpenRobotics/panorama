cmake_minimum_required(VERSION 3.20)

set(CLIENT_SOURCES
    src/main.cpp
    src/mainframe.cpp
    src/argparser.cpp
)

set(CLIENT_HEADERS
    include/client/mainframe.hpp
    include/client/argparser.hpp
)


# Compilation Settings

if(WIN32)
    add_executable(panorama-client WIN32 ${CLIENT_SOURCES} ${CLIENT_HEADERS})
elseif(APPLE)
    add_executable(panorama-client MACOSX_BUNDLE ${CLIENT_SOURCES} ${CLIENT_HEADERS})
else()
    add_executable(panorama-client ${CLIENT_SOURCES} ${CLIENT_HEADERS})
endif()

target_link_libraries(panorama-client 
    PRIVATE 
        ${wxWidgets_LIBRARIES}
)

target_include_directories(panorama-client
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(panorama-client PRIVATE /W4)
    # target_compile_definitions(panorama-client PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(panorama-client PRIVATE -Wall -Wextra -Wpedantic)
endif()


install(TARGETS panorama-client
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# macOS specific configs for creating Info.plist
if(APPLE)
    set_target_properties(panorama-client PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.ubcopenrobotics.panorama"
        MACOSX_BUNDLE_BUNDLE_NAME "Panorama Client"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )
endif()