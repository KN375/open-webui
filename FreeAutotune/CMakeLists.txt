cmake_minimum_required(VERSION 3.19)

project(FreeAutotune VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")

# Audio Unit parameters
set(PLUGIN_CODE "Fatu")
set(PLUGIN_MANUFACTURER_CODE "Free")
set(PLUGIN_NAME "FreeAutotune")
set(PLUGIN_DESCRIPTION "High-performance free autotune plugin")

# Sources
set(SOURCES
    Source/AudioUnitViewController.swift
    Source/FreeAutotuneAU.swift
    Source/DSP/PitchDetector.cpp
    Source/DSP/PitchShifter.cpp
    Source/DSP/AudioProcessor.cpp
    Source/ParameterAddress.swift
)

# Framework
add_library(${PROJECT_NAME} MODULE ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/DSP
)

# Link frameworks
target_link_libraries(${PROJECT_NAME}
    "-framework AudioToolbox"
    "-framework AVFoundation"
    "-framework CoreAudio"
    "-framework Accelerate"
    "-framework SwiftUI"
)

# Set bundle properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    BUNDLE TRUE
    BUNDLE_EXTENSION "appex"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.freeaudio.FreeAutotune"
    XCODE_ATTRIBUTE_SWIFT_VERSION "5.0"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME "YES"
    XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic"
)

# Install to Audio Unit directory
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION "$ENV{HOME}/Library/Audio/Plug-Ins/Components"
)
