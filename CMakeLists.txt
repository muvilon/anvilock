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
        wayland-egl
        EGL 
        GLESv2
        pam
        xkbcommon
        m
)

# Enable warnings
target_compile_options(anvilock PRIVATE -Wall -Wextra -Wpedantic -g)

# Define the config path
set(CONFIG_DIR "$ENV{HOME}/.config/anvilock")
set(CONFIG_FILE "${CONFIG_DIR}/config.toml")

# Create config.toml if it doesn't exist
add_custom_command(
    OUTPUT ${CONFIG_FILE}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CONFIG_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "[font]" > ${CONFIG_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "name = \"# your font name goes here\"" >> ${CONFIG_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "path = \"# your font path goes here\"" >> ${CONFIG_FILE}
    COMMENT "Creating config.toml with placeholder values."
)

add_custom_target(create_config ALL DEPENDS ${CONFIG_FILE})
add_dependencies(anvilock create_config)
