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

# Function to build for macOS (requires macOS or cross-compile setup)
build_macos_cross() {
    echo "Setting up macOS cross-compilation for ARM64..."
    
    # This requires osxcross to be set up
    # See: https://github.com/tpoechtrager/osxcross
    
    BUILD_MACOS="${BUILD_DIR}/build-macos-arm64"
    OUTPUT_MACOS="${OUTPUT_BASE}/macos-arm64"
    
    mkdir -p "${OUTPUT_MACOS}"
    
    cat > "${OUTPUT_MACOS}/README.md" << EOF
# macOS ARM64 Build

To build wxWidgets for macOS ARM64, you need to either:

1. Build on an actual Mac with Apple Silicon, or
2. Set up osxcross for cross-compilation

## Building on macOS:

\`\`\`bash
cd wxWidgets-${WXWIDGETS_VERSION}
mkdir build-mac
cd build-mac

../configure \\
    --prefix=/path/to/output \\
    --enable-unicode \\
    --disable-shared \\
    --enable-monolithic \\
    --with-osx_cocoa \\
    --with-macosx-version-min=11.0 \\
    --enable-universal_binary=arm64 \\
    --disable-debug \\
    --enable-optimise

make -j\$(sysctl -n hw.ncpu)
make install
\`\`\`
EOF
    
    echo "macOS build instructions written to ${OUTPUT_MACOS}/README.md"
    echo "Note: macOS build requires actual Mac hardware or osxcross setup"
}

# Parse command line arguments
if [ "$1" == "linux" ]; then
    build_linux
elif [ "$1" == "windows" ]; then
    build_windows
elif [ "$1" == "macos" ]; then
    build_macos_cross
elif [ "$1" == "all" ]; then
    build_linux
    build_windows
    build_macos_cross
else
    echo "Usage: $0 [linux|windows|macos|all]"
    echo "  linux   - Build for Linux x64"
    echo "  windows - Build for Windows x64 (cross-compile)"
    echo "  macos   - Generate macOS build instructions"
    echo "  all     - Build all platforms"
    exit 1
fi

echo "Build complete! Binaries are in: ${OUTPUT_BASE}"