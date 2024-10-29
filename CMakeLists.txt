cmake_minimum_required(VERSION 3.10)
project(anvilock)

# Set C standard
set(CMAKE_C_STANDARD 11)

# Find Freetype
find_package(Freetype REQUIRED)

# Include directories
include_directories(${FREETYPE_INCLUDE_DIRS} toml)

# Add executable
add_executable(anvilock main.c toml/toml.c)

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

# Generate config.toml if it doesn't exist
add_custom_command(
    OUTPUT config.toml
    COMMAND ${CMAKE_COMMAND} -E echo "[font]" > config.toml
    COMMAND ${CMAKE_COMMAND} -E echo "name = \"# your font name goes here\"" >> config.toml
    COMMAND ${CMAKE_COMMAND} -E echo "path = \"# your font path goes here\"" >> config.toml
    COMMENT "Creating config.toml with placeholder values."
    VERBATIM
)

add_custom_target(create_config DEPENDS config.toml)
add_dependencies(anvilock create_config)
