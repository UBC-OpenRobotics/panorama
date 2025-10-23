# Build wxWidgets for Windows using MSVC
# Run this from PowerShell on Windows with Visual Studio installed

param(
    [string]$Config = "Release"
)

$WXWIDGETS_VERSION = "3.3.1"
$WXWIDGETS_URL = "https://github.com/wxWidgets/wxWidgets/releases/download/v$WXWIDGETS_VERSION/wxWidgets-$WXWIDGETS_VERSION.zip"
$BUILD_DIR = Join-Path $PSScriptRoot "..\build\wxwidgets-build"
$OUTPUT_BASE = Join-Path $PSScriptRoot "..\client\third_party\wxwidgets-prebuilt"
$OUTPUT_WINDOWS = Join-Path $OUTPUT_BASE "windows-x64"

# Create directories
New-Item -ItemType Directory -Force -Path $BUILD_DIR | Out-Null
New-Item -ItemType Directory -Force -Path $OUTPUT_BASE | Out-Null
New-Item -ItemType Directory -Force -Path $OUTPUT_WINDOWS | Out-Null

# Download and extract wxWidgets if not present
$zipFile = Join-Path $BUILD_DIR "wxWidgets-$WXWIDGETS_VERSION.zip"

if (!(Test-Path $zipFile)) {
    Write-Host "Downloading wxWidgets $WXWIDGETS_VERSION..."
    try {
        Invoke-WebRequest -Uri $WXWIDGETS_URL -OutFile $zipFile -UseBasicParsing
    } catch {
        Write-Error "Failed to download wxWidgets: $_"
        exit 1
    }
}

# Extract if not already extracted
$extractCheck = Join-Path $BUILD_DIR "wxWidgets-$WXWIDGETS_VERSION"
if (!(Test-Path $extractCheck)) {
    Write-Host "Extracting wxWidgets..."
    try {
        Expand-Archive -Path $zipFile -DestinationPath $BUILD_DIR -Force
    } catch {
        Write-Error "Failed to extract: $_"
        exit 1
    }
}

# Find the actual wxWidgets source directory
Write-Host "Looking for wxWidgets source..."
$possiblePaths = @(
    (Join-Path $BUILD_DIR "wxWidgets-$WXWIDGETS_VERSION"),
    (Join-Path $BUILD_DIR "wxwidgets-$WXWIDGETS_VERSION"),
    $BUILD_DIR
)

$WXWIDGETS_SRC = $null
foreach ($path in $possiblePaths) {
    $testPath = Join-Path $path "build\msw"
    if (Test-Path $testPath) {
        $WXWIDGETS_SRC = $path
        Write-Host "Found wxWidgets source at: $WXWIDGETS_SRC"
        break
    }
}

# If not found, list what we have and check subdirectories
if ($null -eq $WXWIDGETS_SRC) {
    Write-Host "`nCannot find wxWidgets source. Contents of build directory:"
    Get-ChildItem $BUILD_DIR | Format-Table Name
    
    # Check subdirectories
    $subdirs = Get-ChildItem $BUILD_DIR -Directory
    foreach ($subdir in $subdirs) {
        $testPath = Join-Path $subdir.FullName "build\msw"
        if (Test-Path $testPath) {
            $WXWIDGETS_SRC = $subdir.FullName
            Write-Host "Found wxWidgets in subdirectory: $WXWIDGETS_SRC"
            break
        }
    }
}

if ($null -eq $WXWIDGETS_SRC) {
    Write-Error "Could not locate wxWidgets source with build\msw directory"
    exit 1
}

Write-Host "`nBuilding wxWidgets for Windows x64 with MSVC..."

# Set up Visual Studio environment
$vsPaths = @(
    "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat",
    "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat",
    "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat",
    "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
)

$vsPath = $null
foreach ($path in $vsPaths) {
    if (Test-Path $path) {
        $vsPath = $path
        break
    }
}

if ($null -eq $vsPath) {
    Write-Error "Visual Studio 2022 not found. Please install Visual Studio 2022."
    exit 1
}

Write-Host "Using Visual Studio from: $vsPath"

# Build using MSBuild
$buildMswPath = Join-Path $WXWIDGETS_SRC "build\msw"
Push-Location $buildMswPath

Write-Host "Building in: $buildMswPath"
Write-Host "Running: nmake /f makefile.vc BUILD=release SHARED=0 UNICODE=1 MONOLITHIC=1"

# Use cmd to set up environment and build
$buildCmd = "`"$vsPath`" && nmake /f makefile.vc BUILD=release SHARED=0 UNICODE=1 MONOLITHIC=1"
$result = cmd /c $buildCmd

Pop-Location

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed with exit code $LASTEXITCODE"
    exit 1
}

# Copy output files
Write-Host "`nCopying binaries to $OUTPUT_WINDOWS..."

# Copy include files
$includeDir = Join-Path $OUTPUT_WINDOWS "include\wx-3.3"
$wxIncludeSrc = Join-Path $WXWIDGETS_SRC "include\wx"
New-Item -ItemType Directory -Force -Path $includeDir | Out-Null

if (Test-Path $wxIncludeSrc) {
    Write-Host "Copying include files..."
    Copy-Item -Path "$wxIncludeSrc\*" -Destination (Join-Path $includeDir "wx") -Recurse -Force
} else {
    Write-Warning "Include directory not found at: $wxIncludeSrc"
}

# Copy setup.h - it could be in different locations depending on build
$setupLocations = @(
    (Join-Path $WXWIDGETS_SRC "lib\vc_x64_lib\mswu\wx\setup.h"),
    (Join-Path $WXWIDGETS_SRC "lib\vc_lib\mswu\wx\setup.h")
)

$setupFound = $false
foreach ($setupSrc in $setupLocations) {
    if (Test-Path $setupSrc) {
        Write-Host "Copying setup.h from: $setupSrc"
        $setupDest = Join-Path $OUTPUT_WINDOWS "lib\mswu\wx"
        New-Item -ItemType Directory -Force -Path $setupDest | Out-Null
        Copy-Item -Path $setupSrc -Destination $setupDest -Force
        $setupFound = $true
        break
    }
}

if (!$setupFound) {
    Write-Warning "setup.h not found in expected locations"
}

# Copy library files
$libLocations = @(
    (Join-Path $WXWIDGETS_SRC "lib\vc_x64_lib"),
    (Join-Path $WXWIDGETS_SRC "lib\vc_lib")
)

$libDir = Join-Path $OUTPUT_WINDOWS "lib"
New-Item -ItemType Directory -Force -Path $libDir | Out-Null

$libsFound = $false
foreach ($libSrc in $libLocations) {
    if (Test-Path $libSrc) {
        Write-Host "Copying libraries from: $libSrc"
        Copy-Item -Path "$libSrc\*.lib" -Destination $libDir -Force
        $libsFound = $true
        break
    }
}

if (!$libsFound) {
    Write-Warning "Library files not found in expected locations"
}

Write-Host "`nBuild complete! Libraries are in: $OUTPUT_WINDOWS"
Write-Host "`nLibrary files:"
if (Test-Path $libDir) {
    Get-ChildItem "$libDir\*.lib" | Format-Table Name, Length
} else {
    Write-Warning "Library directory not created"
}

Write-Host "`nTo use these libraries, update your CMakeLists.txt to use MSVC configuration."