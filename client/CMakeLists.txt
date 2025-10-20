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

# Executable settings
if(WIN32)
    add_executable(panorama-client WIN32 ${CLIENT_SOURCES} ${CLIENT_HEADERS})
elseif(APPLE)
    add_executable(panorama-client MACOSX_BUNDLE ${CLIENT_SOURCES} ${CLIENT_HEADERS})
else()
    add_executable(panorama-client ${CLIENT_SOURCES} ${CLIENT_HEADERS})
endif()

# Panorama Includes
target_include_directories(panorama-client
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# wxWidgets Configuration
if(WIN32)
    set(WXWIDGETS_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/wxwidgets-prebuilt/windows-x64")
    set(WXWIDGETS_LIB_PREFIX "lib")
    set(WXWIDGETS_LIB_SUFFIX ".a")
elseif(APPLE)
    set(WXWIDGETS_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/wxwidgets-prebuilt/macos-arm64")
    set(WXWIDGETS_LIB_PREFIX "lib")
    set(WXWIDGETS_LIB_SUFFIX ".a")
else()
    set(WXWIDGETS_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/wxwidgets-prebuilt/linux-x64")
    set(WXWIDGETS_LIB_PREFIX "lib")
    set(WXWIDGETS_LIB_SUFFIX ".a")
endif()

set(wxWidgets_ROOT_DIR "${WXWIDGETS_PREBUILT_PATH}")
set(wxWidgets_LIB_DIR "${WXWIDGETS_PREBUILT_PATH}/lib")
set(wxWidgets_INCLUDE_DIR "${WXWIDGETS_PREBUILT_PATH}/include/wx-3.3")

set(wxWidgets_CONFIG_EXECUTABLE "${WXWIDGETS_PREBUILT_PATH}/bin/wx-config")

if(NOT EXISTS "${wxWidgets_CONFIG_EXECUTABLE}")
    message(FATAL_ERROR "wx-config not found at ${wxWidgets_CONFIG_EXECUTABLE}")
endif()


if(WIN32)
    # Windows static libraries
    target_link_libraries(wxWidgets::wxWidgets INTERFACE
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wx_mswu-3.3${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxregexu-3.3${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxexpat-3.3${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxtiff-3.3${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxjpeg-3.3${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxpng-3.3${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxzlib-3.3${WXWIDGETS_LIB_SUFFIX}"
        # Windows system libraries
        kernel32 user32 gdi32 comdlg32 winspool winmm shell32 shlwapi comctl32 ole32 
        oleaut32 uuid rpcrt4 advapi32 version ws2_32 wininet oleacc uxtheme
    )
    target_compile_definitions(wxWidgets::wxWidgets INTERFACE
        __WXMSW__
        WXUSINGDLL=0
        _UNICODE
        UNICODE
    )
elseif(APPLE)

    execute_process(
        COMMAND ${wxWidgets_CONFIG_EXECUTABLE} --cxxflags
        OUTPUT_VARIABLE WX_CXX_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${wxWidgets_CONFIG_EXECUTABLE} --libs
        OUTPUT_VARIABLE WX_LINK_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_CXX_FLAGS}")
    set(WX_LIBRARIES "${WX_LINK_FLAGS}")
    target_link_libraries(panorama-client "${WX_LIBRARIES}")

    message(STATUS "Using wxWidgets prebuilt from ${WXWIDGETS_PREBUILT_PATH}")
else()
    # Linux static libraries
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    
    target_link_libraries(wxWidgets::wxWidgets INTERFACE
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wx_gtk3u-3.3${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wx_gtk3u_gl-3.3${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxlexilla-3.3${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxscintilla-3.3${WXWIDGETS_LIB_SUFFIX}"


        # "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxregexu-3.3${WXWIDGETS_LIB_SUFFIX}"
        # "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxexpat-3.3${WXWIDGETS_LIB_SUFFIX}"
        # "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxtiff-3.3${WXWIDGETS_LIB_SUFFIX}"
        # "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxjpeg-3.3${WXWIDGETS_LIB_SUFFIX}"
        # "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxpng-3.3${WXWIDGETS_LIB_SUFFIX}"
        # "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxzlib-3.3${WXWIDGETS_LIB_SUFFIX}"
        ${GTK3_LIBRARIES}
        pthread
        dl
        m
    )
    target_include_directories(wxWidgets::wxWidgets INTERFACE
        ${GTK3_INCLUDE_DIRS}
    )
    target_compile_definitions(wxWidgets::wxWidgets INTERFACE
        __WXGTK__
        __WXGTK3__
        WXUSINGDLL=0
    )
endif()

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