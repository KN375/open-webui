#!/bin/bash

# RealTune Build Script (AUV3 Version)
# Builds and installs the Audio Unit V3 plugin for Logic Pro

set -e

echo "======================================"
echo "RealTune Build Script (AUV3)"
echo "======================================"
echo ""
echo "⚠️  IMPORTANT:"
echo "This script requires a properly configured Xcode project."
echo "Please follow BUILD_INSTRUCTIONS.md for detailed setup."
echo ""

# Configuration
PROJECT_NAME="RealTune"
BUILD_DIR="build"
APP_NAME="${PROJECT_NAME}.app"
AU_EXTENSION="${PROJECT_NAME}AU.appex"
INSTALL_DIR="/Applications"
XCODE_PROJECT="${PROJECT_NAME}.xcodeproj"

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

# Check for Xcode project
if [ ! -d "$XCODE_PROJECT" ]; then
    echo -e "${RED}Error: Xcode project not found${NC}"
    echo ""
    echo "Please create the Xcode project first:"
    echo "1. Follow instructions in BUILD_INSTRUCTIONS.md"
    echo "2. Or use: ruby generate_xcode_project.rb"
    echo ""
    exit 1
fi

# Clean previous build (optional)
echo "Do you want to clean previous build? (y/N)"
read -r response
if [[ "$response" =~ ^([yY][eE][sS]|[yY])$ ]]; then
    if [ -d "$BUILD_DIR" ]; then
        echo "Cleaning previous build..."
        rm -rf "$BUILD_DIR"
    fi
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Build the project
echo ""
echo "Building RealTune with Xcode..."
echo "This may take a few minutes..."
echo ""

xcodebuild -project "$XCODE_PROJECT" \
           -scheme "$PROJECT_NAME" \
           -configuration Release \
           -derivedDataPath "$BUILD_DIR" \
           build

if [ $? -ne 0 ]; then
    echo -e "${RED}Error: Build failed${NC}"
    echo ""
    echo "Common issues:"
    echo "1. Missing source files in Xcode project"
    echo "2. Incorrect bridging header path"
    echo "3. Code signing issues"
    echo ""
    echo "Please check BUILD_INSTRUCTIONS.md for detailed setup."
    exit 1
fi

# Find built app
BUILT_APP=$(find "$BUILD_DIR" -name "$APP_NAME" -type d | head -1)

if [ -z "$BUILT_APP" ]; then
    echo -e "${RED}Error: Built application not found${NC}"
    echo "Expected location: $BUILD_DIR/Build/Products/Release/$APP_NAME"
    exit 1
fi

echo "Found built app: $BUILT_APP"

# Verify Audio Unit Extension exists
AU_PATH="$BUILT_APP/Contents/PlugIns/$AU_EXTENSION"
if [ ! -d "$AU_PATH" ]; then
    echo -e "${RED}Error: Audio Unit Extension not found in app bundle${NC}"
    echo "Expected: $AU_PATH"
    exit 1
fi

# Remove old installation if exists
if [ -d "$INSTALL_DIR/$APP_NAME" ]; then
    echo "Removing old installation..."
    sudo rm -rf "$INSTALL_DIR/$APP_NAME"
fi

# Copy new app
echo "Copying app to $INSTALL_DIR..."
sudo cp -R "$BUILT_APP" "$INSTALL_DIR/"

# Verify installation
if [ -d "$INSTALL_DIR/$APP_NAME" ]; then
    echo ""
    echo -e "${GREEN}======================================"
    echo "Build and installation successful!"
    echo "======================================${NC}"
    echo ""
    echo "Application installed to:"
    echo "  $INSTALL_DIR/$APP_NAME"
    echo ""
    echo "Audio Unit Extension located at:"
    echo "  $INSTALL_DIR/$APP_NAME/Contents/PlugIns/$AU_EXTENSION"
    echo ""
    echo "Next steps:"
    echo "  1. Launch Logic Pro"
    echo "  2. Create or open a project"
    echo "  3. Add RealTune to an audio track:"
    echo "     Audio FX → Audio Units → RealTune"
    echo ""
    echo "To uninstall:"
    echo "  sudo rm -rf \"$INSTALL_DIR/$APP_NAME\""
    echo ""
else
    echo -e "${RED}Error: Installation failed${NC}"
    exit 1
fi

# Reset audio system (optional, helps Logic Pro detect new plugin)
echo "Resetting audio component cache..."
killall -9 AudioComponentRegistrar 2>/dev/null || true

echo -e "${GREEN}Done!${NC}"
