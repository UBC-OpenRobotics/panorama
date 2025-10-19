cmake_minimum_required(VERSION 3.20)
project(panorama LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_CLIENT "Build Panorama-client application" ON)
option(BUILD_TESTS "Build unit tests" ON)

# add_subdirectory(common)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

if(WIN32)
    set(WXWIDGETS_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/client/third_party/wxwidgets-prebuilt/windows-x64")
    set(WXWIDGETS_LIB_PREFIX "lib")
    set(WXWIDGETS_LIB_SUFFIX ".a")
elseif(APPLE)
    set(WXWIDGETS_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/client/third_party/wxwidgets-prebuilt/macos-arm64")
    set(WXWIDGETS_LIB_PREFIX "lib")
    set(WXWIDGETS_LIB_SUFFIX ".a")
else()
    set(WXWIDGETS_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/client/third_party/wxwidgets-prebuilt/linux-x64")
    set(WXWIDGETS_LIB_PREFIX "lib")
    set(WXWIDGETS_LIB_SUFFIX ".a")
endif()

set(wxWidgets_ROOT_DIR "${WXWIDGETS_PREBUILT_PATH}")
set(wxWidgets_LIB_DIR "${WXWIDGETS_PREBUILT_PATH}/lib")
set(wxWidgets_INCLUDE_DIR "${WXWIDGETS_PREBUILT_PATH}/include/wx-3.3")

add_library(wxWidgets::wxWidgets INTERFACE IMPORTED)

target_include_directories(wxWidgets::wxWidgets INTERFACE
    "${WXWIDGETS_PREBUILT_PATH}/include/wx-3.3"
    "${WXWIDGETS_PREBUILT_PATH}/lib/wx/include/gtk3-unicode-static-3.3"  # Linux specific, adjust for other platforms
)

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
    # macOS static libraries
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(CARBON_LIBRARY Carbon)
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
    find_library(SYSTEM_LIBRARY System)
    find_library(OPENGL_LIBRARY OpenGL)
    
    target_link_libraries(wxWidgets::wxWidgets INTERFACE
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wx_osx_cocoau-3.2${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxregexu-3.2${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxexpat-3.2${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxtiff-3.2${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxjpeg-3.2${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxpng-3.2${WXWIDGETS_LIB_SUFFIX}"
        "${wxWidgets_LIB_DIR}/${WXWIDGETS_LIB_PREFIX}wxzlib-3.2${WXWIDGETS_LIB_SUFFIX}"
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${CARBON_LIBRARY}
        ${AUDIOTOOLBOX_LIBRARY}
        ${SYSTEM_LIBRARY}
        ${OPENGL_LIBRARY}
    )
    target_compile_definitions(wxWidgets::wxWidgets INTERFACE
        __WXOSX__
        __WXOSX_COCOA__
        WXUSINGDLL=0
    )
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

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

