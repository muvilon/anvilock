cmake_minimum_required(VERSION 3.25)
project(example_toml LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES main.cpp)

# Executable
add_executable(example_toml ${SOURCES})

set_target_properties(example_toml PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
)

# Include directories
target_include_directories(example_toml PRIVATE ${CMAKE_SOURCE_DIR}/../)

add_custom_command(TARGET example_toml POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/examples/toml/example.toml
            ${CMAKE_BINARY_DIR}/examples/example.toml
    COMMENT "==> Copying example.toml to output directory"
)

