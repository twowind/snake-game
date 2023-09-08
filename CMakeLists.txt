# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.0.0)

# Define the project and its version
project(snake VERSION 0.1.0)

# Set the C++ standard for compilation
set(CMAKE_CXX_STANDARD 20)

# Generate compile_commands.json for better tooling support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Globbing source files in the "./src/" directory
file(GLOB TARGET_SRC "./src/*.cpp")

# Include the ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Find and include the ncurses panel library
find_library(PANEL_LIB panel REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Find and include the ncurses menu library
find_library(MENU_LIB menu REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Create an executable named "main" from the source files
add_executable(main ${TARGET_SRC})

# Link ncurses-related libraries to your executable
target_link_libraries(main PRIVATE ${CURSES_LIBRARIES} ${MENU_LIB} ${PANEL_LIB})

