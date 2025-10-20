#!/bin/bash

# Build wxWidgets for Linux, Windows, and macOS
# Place the results in client/third_party/wxwidgets-prebuilt/

set -e

WXWIDGETS_VERSION="3.3.1"
WXWIDGETS_URL="https://github.com/wxWidgets/wxWidgets/releases/download/v${WXWIDGETS_VERSION}/wxWidgets-${WXWIDGETS_VERSION}.tar.bz2"
BUILD_DIR="$(pwd)/../build/wxwidgets-build"
OUTPUT_BASE="$(pwd)/../client/third_party/wxwidgets-prebuilt"

mkdir -p "${BUILD_DIR}"
mkdir -p "${OUTPUT_BASE}"

# Download wxWidgets source if not present
if [ ! -f "${BUILD_DIR}/wxWidgets-${WXWIDGETS_VERSION}.tar.bz2" ]; then
    echo "Downloading wxWidgets ${WXWIDGETS_VERSION}..."
    cd "${BUILD_DIR}"
    wget "${WXWIDGETS_URL}"
    tar -xjf "wxWidgets-${WXWIDGETS_VERSION}.tar.bz2"
fi

WXWIDGETS_SRC="${BUILD_DIR}/wxWidgets-${WXWIDGETS_VERSION}"

build_linux() {
    echo "Building wxWidgets for Linux x64..."
    
    BUILD_LINUX="${BUILD_DIR}/build-linux-x64"
    OUTPUT_LINUX="${OUTPUT_BASE}/linux-x64"
    
    rm -rf "${BUILD_LINUX}"
    mkdir -p "${BUILD_LINUX}"
    mkdir -p "${OUTPUT_LINUX}"
    
    cd "${BUILD_LINUX}"
    
    "${WXWIDGETS_SRC}/configure" \
        --prefix="${OUTPUT_LINUX}" \
        --enable-shared \
        --disable-shared \
        --with-gtk=3 \
        --enable-webview \
        --enable-mediactrl \
        --with-opengl \
        --enable-graphics_ctx \
        --enable-monolithic \
        --disable-debug \
        --enable-optimise \
        CXXFLAGS="-std=c++11 -fPIC" \
        CFLAGS="-fPIC"
    
    make -j$(nproc)
    make install
    
    # Create a simplified wx-config wrapper
    cat > "${OUTPUT_LINUX}/wx-config" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
exec "${SCRIPT_DIR}/bin/wx-config" "$@"
EOF
    chmod +x "${OUTPUT_LINUX}/wx-config"
    
    echo "Linux build complete!"
}

build_windows() {
    # (cross compile with MinGW)
    echo "Building wxWidgets for Windows x64..."
    
    # Check if MinGW cross-compiler is installed
    if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
        echo "MinGW cross-compiler not found. Installing..."
        sudo apt-get update
        sudo apt-get install -y mingw-w64
    fi
    
    BUILD_WINDOWS="${BUILD_DIR}/build-windows-x64"
    OUTPUT_WINDOWS="${OUTPUT_BASE}/windows-x64"
    
    rm -rf "${BUILD_WINDOWS}"
    mkdir -p "${BUILD_WINDOWS}"
    mkdir -p "${OUTPUT_WINDOWS}"
    
    cd "${BUILD_WINDOWS}"
    
    "${WXWIDGETS_SRC}/configure" \
        --prefix="${OUTPUT_WINDOWS}" \
        --host=x86_64-w64-mingw32 \
        --build=x86_64-linux-gnu \
        --disable-shared \
        --enable-monolithic \
        --with-msw \
        --with-opengl \
        --enable-graphics_ctx \
        --disable-debug \
        --enable-optimise \
        CXXFLAGS="-std=c++11" \
        LDFLAGS="-static"
    
    make -j$(nproc)
    make install
    
    echo "Windows build complete!"
}

build_macos() {
    echo "Building wxWidgets for macOS ARM64..."
    
    BUILD_MACOS="${BUILD_DIR}/build-macos-arm64"
    OUTPUT_MACOS="${OUTPUT_BASE}/macos-arm64"
    
    rm -rf "${BUILD_MACOS}"
    mkdir -p "${BUILD_MACOS}"
    mkdir -p "${OUTPUT_MACOS}"
    
    cd "${BUILD_MACOS}"
    
    ARCH=$(uname -m)
    if [ "$ARCH" = "arm64" ]; then
        echo "Building for native ARM64..."
        ARCH_FLAGS="--enable-macosx_arch=arm64"
    else
        echo "Building universal binary for ARM64 on Intel Mac..."
        ARCH_FLAGS="--enable-universal_binary=arm64,x86_64"
    fi
    
    "${WXWIDGETS_SRC}/configure" \
        --prefix="${OUTPUT_MACOS}" \
        --disable-shared \
        --enable-monolithic \
        --with-osx_cocoa \
        --with-macosx-version-min=11.0 \
        ${ARCH_FLAGS} \
        --enable-webview \
        --with-opengl \
        --enable-graphics_ctx \
        --disable-debug \
        --enable-optimise \
        --disable-sys-libs \
        --with-libjpeg=builtin \
        --with-libpng=builtin \
        --with-libtiff=builtin \
        --with-zlib=builtin \
        --with-expat=builtin \
        CXXFLAGS="-std=c++11" \
        CFLAGS="-fPIC" \
        CPPFLAGS="-fPIC" \
        OBJCXXFLAGS="-std=c++11"
    
    make -j$(sysctl -n hw.ncpu)
    make install
    
    cat > "${OUTPUT_MACOS}/wx-config" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
exec "${SCRIPT_DIR}/bin/wx-config" "$@"
EOF
    chmod +x "${OUTPUT_MACOS}/wx-config"
    
    cat > "${OUTPUT_MACOS}/build_info.txt" << EOF
wxWidgets ${WXWIDGETS_VERSION} for macOS ARM64
Built on: $(date)
Architecture: ${ARCH}
Min macOS version: 11.0
Configuration: Static, Monolithic, Optimized
EOF
    
    echo "macOS ARM64 build complete!"
    echo "Binaries installed to: ${OUTPUT_MACOS}"
}

# Parse command line arguments
if [ "$1" == "linux" ]; then
    build_linux
elif [ "$1" == "windows" ]; then
    build_windows
elif [ "$1" == "macos" ]; then
    build_macos
elif [ "$1" == "all" ]; then
    build_linux
    build_windows
    build_macos
else
    echo "Usage: $0 [linux|windows|macos|all]"
    echo "  linux   - Build for Linux x64"
    echo "  windows - Build for Windows x64 (cross-compile)"
    echo "  macos   - Build for macOS"
    echo "  all     - Build all platforms"
    exit 1
fi

echo "Build complete! Binaries are in: ${OUTPUT_BASE}"