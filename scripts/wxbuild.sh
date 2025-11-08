#!/bin/bash
set -e

# Configuration
WX_VERSION="3.2.8"
WX_URL="https://github.com/wxWidgets/wxWidgets/releases/download/v${WX_VERSION}/wxWidgets-${WX_VERSION}.tar.bz2"
INSTALL_PREFIX="/usr/local"
BUILD_DIR="${HOME}/wxwidgets-build"

if [[ "$OSTYPE" == "darwin"* ]]; then
    OS_NAME="macOS"
    NUM_CORES=$(sysctl -n hw.ncpu)
else
    OS_NAME="Linux"
    NUM_CORES=$(nproc)
fi

echo "========================================"
echo "wxWidgets ${WX_VERSION} Build Script"
echo "========================================"
echo "Operating System: ${OS_NAME}"
echo "Install prefix: ${INSTALL_PREFIX}"
echo "Build directory: ${BUILD_DIR}"
echo "Using ${NUM_CORES} cores for compilation"
echo ""

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Download wxWidgets if not already present
if [ ! -f "wxWidgets-${WX_VERSION}.tar.bz2" ]; then
    echo "Downloading wxWidgets ${WX_VERSION}..."
    curl -L -o "wxWidgets-${WX_VERSION}.tar.bz2" "${WX_URL}"
else
    echo "wxWidgets archive already exists, skipping download..."
fi

# Extract
echo "Extracting archive..."
if [ -d "wxWidgets-${WX_VERSION}" ]; then
    echo "Removing existing source directory..."
    rm -rf "wxWidgets-${WX_VERSION}"
fi
tar -xjf "wxWidgets-${WX_VERSION}.tar.bz2"

# Build
cd "wxWidgets-${WX_VERSION}"
mkdir -p build-release
cd build-release

echo "Configuring wxWidgets..."

# Configure with OS-specific options
if [[ "$OSTYPE" == "darwin"* ]]; then
    ../configure \
        --prefix="${INSTALL_PREFIX}" \
        --enable-optimise \
        --disable-debug \
        --enable-unicode \
        --enable-std_string \
        --enable-display \
        --with-libjpeg \
        --with-libpng \
        --with-regex \
        --with-libtiff \
        --with-zlib \
        --with-expat \
        --without-liblzma \
        --with-macosx-version-min=10.15 \
        --enable-cxx11 \
        --with-cxx=14
else
    # Linux configuration
    ../configure \
        --prefix="${INSTALL_PREFIX}" \
        --enable-optimise \
        --disable-debug \
        --enable-unicode \
        --enable-std_string \
        --enable-display \
        --with-libjpeg \
        --with-libpng \
        --with-regex \
        --with-libtiff \
        --with-zlib \
        --with-expat \
        --without-liblzma \
        --enable-cxx11 \
        --with-cxx=14 \
        --with-gtk=3
fi

echo ""
echo "Building wxWidgets (this may take several minutes)..."
make -j${NUM_CORES}

echo ""
echo "Installing wxWidgets to ${INSTALL_PREFIX}..."
echo "Note: This requires sudo privileges"
sudo make install

# Update library cache (OS-specific)
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo ""
    echo "Updating dynamic library cache..."
    sudo update_dyld_shared_cache
else
    echo ""
    echo "Updating library cache..."
    sudo ldconfig
fi

echo ""
echo "========================================"
echo "Build and installation complete!"
echo "========================================"
echo ""
echo "To use wxWidgets in your projects:"
echo "  - Use 'wx-config --cxxflags' for compiler flags"
echo "  - Use 'wx-config --libs' for linker flags"
echo ""
echo "Example CMake configuration:"
echo "  find_package(wxWidgets REQUIRED COMPONENTS core base)"
echo "  include(\${wxWidgets_USE_FILE})"
echo "  target_link_libraries(your_target \${wxWidgets_LIBRARIES})"
echo ""
echo "Verify installation:"
echo "  wx-config --version"
echo ""
echo "Build directory can be cleaned up at: ${BUILD_DIR}"