cmake_minimum_required(VERSION 3.20)

set(CLIENT_SOURCES
    src/main.cpp
    src/mainframe.cpp
    src/argparser.cpp
    src/message_model.cpp
    src/tcp_client.cpp
    src/json_reader.cpp
)

set(CLIENT_HEADERS
    include/client/panorama_defines.hpp
    include/client/mainframe.hpp
    include/client/argparser.hpp
    include/client/message_model.hpp
    include/client/tcp_client.hpp
    include/client/json_reader.hpp
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

if(WIN32)
    target_link_libraries(panorama-client PRIVATE ws2_32)
endif()


# wxWidgets Configuration
if(WIN32)
    set(WXWIDGETS_PREBUILT_PATH "$ENV{WXWIN}")
elseif(APPLE)
    set(WXWIDGETS_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/wxwidgets-prebuilt/macos-arm64")
else()
    set(WXWIDGETS_PREBUILT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/wxwidgets-prebuilt/linux-x64")
endif()

set(wxWidgets_ROOT_DIR "${WXWIDGETS_PREBUILT_PATH}")
set(wxWidgets_LIB_DIR "${WXWIDGETS_PREBUILT_PATH}/lib")
set(wxWidgets_INCLUDE_DIR "${WXWIDGETS_PREBUILT_PATH}/include")

if(WIN32)
    # Detect if using MinGW or MSVC
    if(MINGW)
        message(STATUS "Configuring for MinGW build")
        
        target_include_directories(panorama-client PRIVATE 
            "${wxWidgets_INCLUDE_DIR}"
            "${WXWIDGETS_PREBUILT_PATH}/lib/wx/include/x86_64-w64-mingw32-msw-unicode-static-3.3"
        )
        
        target_link_libraries(panorama-client PRIVATE
            "${wxWidgets_LIB_DIR}/libwx_mswu-3.3-x86_64-w64-mingw32.a"
            "${wxWidgets_LIB_DIR}/libwxregexu-3.3-x86_64-w64-mingw32.a"
            "${wxWidgets_LIB_DIR}/libwxexpat-3.3-x86_64-w64-mingw32.a"
            "${wxWidgets_LIB_DIR}/libwxtiff-3.3-x86_64-w64-mingw32.a"
            "${wxWidgets_LIB_DIR}/libwxjpeg-3.3-x86_64-w64-mingw32.a"
            "${wxWidgets_LIB_DIR}/libwxpng-3.3-x86_64-w64-mingw32.a"
            "${wxWidgets_LIB_DIR}/libwxzlib-3.3-x86_64-w64-mingw32.a"
            # Windows system libraries
            -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lwinspool -lwinmm -lshell32 
            -lshlwapi -lcomctl32 -lole32 -loleaut32 -luuid -lrpcrt4 -ladvapi32 
            -lversion -lws2_32 -lwininet -loleacc -luxtheme
        )
        
        target_compile_definitions(panorama-client PRIVATE
            __WXMSW__
            WXUSINGDLL=0
            _UNICODE
            UNICODE
        )
        
    elseif(MSVC)
        message(STATUS "Configuring for MSVC build")
        
        target_include_directories(panorama-client PRIVATE 
            "${wxWidgets_INCLUDE_DIR}"
            "${wxWidgets_LIB_DIR}/vc_x64_lib/mswu"
        )
        
        # Find all wxWidgets libraries in the lib directory
        file(GLOB WX_LIBS "${wxWidgets_LIB_DIR}/vc_x64_lib/wx*.lib")
        
        if(NOT WX_LIBS)
            message(FATAL_ERROR "No wxWidgets libraries found in ${wxWidgets_LIB_DIR}")
        endif()
        
        message(STATUS "Found wxWidgets libraries:")
        foreach(lib ${WX_LIBS})
            get_filename_component(lib_name ${lib} NAME)
            message(STATUS "  ${lib_name}")
        endforeach()
        
        target_link_libraries(panorama-client PRIVATE
            ${WX_LIBS}
            # Windows system libraries
            kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib 
            shell32.lib shlwapi.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib 
            rpcrt4.lib advapi32.lib version.lib ws2_32.lib wininet.lib oleacc.lib 
            uxtheme.lib gdiplus.lib
        )
        
        target_compile_definitions(panorama-client PRIVATE
            __WXMSW__
            _UNICODE
            UNICODE
        )
    endif()

    message(STATUS "Using wxWidgets prebuilt from ${WXWIDGETS_PREBUILT_PATH}")
    
elseif(APPLE)
    # macOS: Use system-installed wxWidgets from /usr/local
    find_program(wxWidgets_CONFIG_EXECUTABLE wx-config
        HINTS /usr/local/bin
        DOC "Location of wxWidgets wx-config utility"
    )
    
    if(NOT wxWidgets_CONFIG_EXECUTABLE)
        message(FATAL_ERROR 
            "wx-config not found. Please install wxWidgets:\n"
            "  Run the build_wxwidgets_universal.sh script\n"
            "  Or install via Homebrew: brew install wxwidgets"
        )
    endif()

    execute_process(
        COMMAND ${wxWidgets_CONFIG_EXECUTABLE} --version
        OUTPUT_VARIABLE WX_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
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

    message(STATUS "Found wxWidgets version: ${WX_VERSION}")
    message(STATUS "Using wx-config: ${wxWidgets_CONFIG_EXECUTABLE}")

    # Apply wxWidgets compiler flags
    separate_arguments(WX_CXX_FLAGS_LIST UNIX_COMMAND "${WX_CXX_FLAGS}")
    target_compile_options(panorama-client PRIVATE ${WX_CXX_FLAGS_LIST})
    
    # Apply wxWidgets linker flags - don't use separate_arguments to preserve framework syntax
    string(STRIP "${WX_LINK_FLAGS}" WX_LINK_FLAGS)
    target_link_libraries(panorama-client PRIVATE ${WX_LINK_FLAGS})

    message(STATUS "Using wxWidgets prebuilt from ${WXWIDGETS_PREBUILT_PATH}")
else() # linux
	

	find_program(wxWidgets_CONFIG_EXECUTABLE wx-config)
    #set(wxWidgets_CONFIG_EXECUTABLE "${WXWIDGETS_PREBUILT_PATH}/bin/wx-config")
 
    if(NOT EXISTS "${wxWidgets_CONFIG_EXECUTABLE}")
        message(FATAL_ERROR "wx-config not found.")
    endif()

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

    message(STATUS "Using locally built wxWidgets via wx-config: ${wxWidgets_CONFIG_EXECUTABLE}")
endif()

if(MSVC)
    target_compile_options(panorama-client PRIVATE /W4)
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
