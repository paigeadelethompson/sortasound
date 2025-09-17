cmake_minimum_required(VERSION 3.16)

project(FMSynthesizer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia)

# Enable Qt6 MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/window/main.cpp
    src/window/setup.cpp
    src/window/controls.cpp
    src/window/synthesizer.cpp
    src/window/internals.cpp
    src/widget/keyboard.cpp
    src/window/tracker.cpp
    src/fm.cpp
    src/algorithms.cpp
    src/presets.cpp
)

# Header files
set(HEADERS
    include/window/main.hpp
    include/widget/keyboard.hpp
    include/window/tracker.hpp
    include/fm.hpp
    include/presets.hpp
)

# Create executable
add_executable(FMSynthesizer ${SOURCES} ${HEADERS})

# Link Qt6 libraries
target_link_libraries(FMSynthesizer
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
)