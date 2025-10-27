#!/bin/bash

# FreeAutotune Build Script
# Builds and installs the Audio Unit plugin for Logic Pro

set -e

echo "======================================"
echo "FreeAutotune Build Script"
echo "======================================"
echo ""

# Configuration
BUILD_DIR="build"
INSTALL_DIR="$HOME/Library/Audio/Plug-Ins/Components"
PLUGIN_NAME="FreeAutotune.component"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo -e "${RED}Error: This script must be run on macOS${NC}"
    exit 1
fi

# Check for Xcode command line tools
if ! command -v xcodebuild &> /dev/null; then
    echo -e "${RED}Error: Xcode command line tools not found${NC}"
    echo "Please install with: xcode-select --install"
    exit 1
fi

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${YELLOW}Warning: CMake not found. Installing via Homebrew...${NC}"
    if command -v brew &> /dev/null; then
        brew install cmake
    else
        echo -e "${RED}Error: Homebrew not found. Please install CMake manually${NC}"
        exit 1
    fi
fi

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
echo "Creating build directory..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Generate Xcode project
echo ""
echo "Generating Xcode project..."
cmake -G Xcode ..

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: CMake configuration failed${NC}"
    exit 1
fi

# Build the project
echo ""
echo "Building FreeAutotune..."
echo "This may take a few minutes..."
xcodebuild -project FreeAutotune.xcodeproj \
           -scheme FreeAutotune \
           -configuration Release \
           build

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: Build failed${NC}"
    exit 1
fi

# Create install directory if it doesn't exist
echo ""
echo "Installing plugin..."
mkdir -p "$INSTALL_DIR"

# Find built component
BUILT_COMPONENT=$(find . -name "$PLUGIN_NAME" -type d | head -1)

if [ -z "$BUILT_COMPONENT" ]; then
    echo -e "${RED}Error: Built component not found${NC}"
    exit 1
fi

# Remove old installation if exists
if [ -d "$INSTALL_DIR/$PLUGIN_NAME" ]; then
    echo "Removing old installation..."
    rm -rf "$INSTALL_DIR/$PLUGIN_NAME"
fi

# Copy new plugin
echo "Copying plugin to $INSTALL_DIR..."
cp -R "$BUILT_COMPONENT" "$INSTALL_DIR/"

# Verify installation
if [ -d "$INSTALL_DIR/$PLUGIN_NAME" ]; then
    echo ""
    echo -e "${GREEN}======================================"
    echo "Build and installation successful!"
    echo "======================================${NC}"
    echo ""
    echo "Plugin installed to:"
    echo "  $INSTALL_DIR/$PLUGIN_NAME"
    echo ""
    echo "Next steps:"
    echo "  1. Launch Logic Pro"
    echo "  2. Create or open a project"
    echo "  3. Add FreeAutotune to an audio track:"
    echo "     Audio FX → Audio Units → FreeAutotune"
    echo ""
    echo "To uninstall:"
    echo "  rm -rf \"$INSTALL_DIR/$PLUGIN_NAME\""
    echo ""
else
    echo -e "${RED}Error: Installation failed${NC}"
    exit 1
fi

# Reset audio system (optional, helps Logic Pro detect new plugin)
echo "Resetting audio component cache..."
killall -9 AudioComponentRegistrar 2>/dev/null || true

echo -e "${GREEN}Done!${NC}"
