cmake_minimum_required(VERSION 3.10)
project(anvilock)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Find Freetype
find_package(Freetype REQUIRED)

# Include directories
include_directories(${FREETYPE_INCLUDE_DIRS})

# Add executable
add_executable(anvilock main.c)

# Link libraries
target_link_libraries(anvilock
    PRIVATE
        ${FREETYPE_LIBRARIES}
        wayland-client
        wayland-server
        pam
        xkbcommon
)

# Enable warnings
target_compile_options(anvilock PRIVATE -Wall -Wextra -Wpedantic -g)
