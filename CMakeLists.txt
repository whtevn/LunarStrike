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
    include_directories(/usr/local/include /opt/homebrew/include /opt/homebrew/opt/ncurses/include)
    link_directories(/usr/local/lib /opt/homebrew/lib /opt/homebrew/opt/ncurses/lib)
    set(PLATFORM_LIBS
        "-framework CoreMIDI"
        "-framework CoreAudio"
        "-framework CoreFoundation"
        portaudio
        portmidi
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

# Force static builds
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libs only" FORCE)
set(RTMIDI_BUILD_SHARED_LIBS OFF CACHE BOOL "RtMidi static only" FORCE)
set(RTMIDI_BUILD_STATIC_LIBS ON CACHE BOOL "RtMidi static" FORCE)
set(YAML_CPP_BUILD_SHARED_LIBS OFF CACHE BOOL "yaml-cpp static only" FORCE)
set(YAML_CPP_BUILD_STATIC_LIBS ON CACHE BOOL "yaml-cpp static" FORCE)

# Add submodules
add_subdirectory(lib/DaisySP)
add_subdirectory(lib/rtmidi)
add_subdirectory(lib/yaml-cpp)

# Collect source files
file(GLOB_RECURSE SOURCES src/*.cpp)

# Add the main executable
add_executable(lunar-strike ${SOURCES})

# Link submodules and platform libs
target_link_libraries(lunar-strike
    PRIVATE
    DaisySP
    rtmidi
    yaml-cpp
    ${PLATFORM_LIBS}
)

