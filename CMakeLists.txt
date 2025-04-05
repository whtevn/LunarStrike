cmake_minimum_required(VERSION 3.16)
project(lunar-strike LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Platform-specific settings
if(APPLE)
    add_compile_definitions(MACOS)
    # Do not add global include/link directories so submodules use their own headers.
    # Remove portmidi since CoreMIDI is used on macOS.
    set(PLATFORM_LIBS
        "-framework CoreMIDI"
        "-framework CoreAudio"
        "-framework CoreFoundation"
        ncurses
    )
else()
    set(PLATFORM_LIBS
        pthread
        stdc++
        ncurses
        tinfo
        portaudio
        m
        asound
    )
endif()

# Force static builds and disable YAML-CPP extra executables
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libs only" FORCE)
set(RTMIDI_BUILD_SHARED_LIBS OFF CACHE BOOL "RtMidi static only" FORCE)
set(RTMIDI_BUILD_STATIC_LIBS ON CACHE BOOL "RtMidi static" FORCE)
set(YAML_CPP_BUILD_SHARED_LIBS OFF CACHE BOOL "yaml-cpp static only" FORCE)
set(YAML_CPP_BUILD_STATIC_LIBS ON CACHE BOOL "yaml-cpp static" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Disable yaml-cpp tools" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable yaml-cpp tests" FORCE)

# Add submodules
add_subdirectory(lib/DaisySP)
add_subdirectory(lib/rtmidi)
add_subdirectory(lib/yaml-cpp)

# Collect source files
file(GLOB_RECURSE SOURCES src/*.cpp)

# Add the main executable
add_executable(lunar-strike ${SOURCES})

# Force the local yaml-cpp include directory to be searched first for the main executable
target_include_directories(lunar-strike BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/yaml-cpp/include)

# Add platform-specific include directories for macOS
if(APPLE)
    # Add ncurses include directory (if needed)
    target_include_directories(lunar-strike PRIVATE /opt/homebrew/opt/ncurses/include)

    # Find PortAudio header and library
    find_path(PORTAUDIO_INCLUDE_DIR portaudio.h
        PATHS /opt/homebrew/include /usr/local/include
    )
    if(PORTAUDIO_INCLUDE_DIR)
        message(STATUS "Found PortAudio include at ${PORTAUDIO_INCLUDE_DIR}")
        target_include_directories(lunar-strike PRIVATE ${PORTAUDIO_INCLUDE_DIR})
    else()
        message(WARNING "portaudio.h not found. Please install PortAudio via Homebrew (brew install portaudio).")
    endif()

    find_library(PORTAUDIO_LIBRARY NAMES portaudio
        PATHS /opt/homebrew/lib /usr/local/lib
    )
    if(PORTAUDIO_LIBRARY)
        message(STATUS "Found PortAudio library at ${PORTAUDIO_LIBRARY}")
        target_link_libraries(lunar-strike PRIVATE ${PORTAUDIO_LIBRARY})
    else()
        message(WARNING "PortAudio library not found. Please install PortAudio via Homebrew (brew install portaudio).")
    endif()
endif()

# Link submodules and platform libraries
target_link_libraries(lunar-strike
    PRIVATE
    DaisySP
    rtmidi
    yaml-cpp
    ${PLATFORM_LIBS}
)

