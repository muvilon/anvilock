cmake_minimum_required(VERSION 3.22)
project(anvilock VERSION 1.5 LANGUAGES C)

# Option to use Ninja as the build system
option(USE_NINJA "Use Ninja build system" OFF)
option(BUILD_GLOBAL "Build Anvilock globally" OFF)

if(USE_NINJA)
    message(STATUS "Using Ninja as the build system...")
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        set(CMAKE_GENERATOR "Ninja" CACHE STRING "Build system generator" FORCE)
    endif()
endif()

if(USE_CLANG)
    message(STATUS "Using Clang as the compiler...")
    set(CMAKE_C_COMPILER clang CACHE STRING "C Compiler" FORCE)
endif()


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

string(ASCII 27 ESC)
set(RESET "${ESC}[0m")
set(GREEN "${ESC}[32m")  # Success messages
set(YELLOW "${ESC}[33m") # Warning messages
set(BLUE   "${ESC}[34m")  # Information messages

# Define paths
set(CONFIG_DIR "$ENV{HOME}/.config/anvilock")
set(CONFIG_FILE "${CONFIG_DIR}/config.toml")

# Enable Debugging and Sanitizer Builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug-ASan")
    set(EXECUTABLE_NAME "anvilock-DBG-ASan")
    message(STATUS "Enabling AddressSanitizer")
    set(SANITIZER_FLAGS "-fsanitize=address -fsanitize=undefined -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug-TSan")
    set(EXECUTABLE_NAME "anvilock-DBG-TSan")
    message(STATUS "Enabling ThreadSanitizer")
    set(SANITIZER_FLAGS "-fsanitize=thread -fsanitize=undefined -g")
else()
    set(CMAKE_BUILD_TYPE "Release")
    set(EXECUTABLE_NAME "anvilock")
    message(STATUS "Skipping Sanitizers for Release build")
endif()

# Enable Warnings
set(COMPILE_OPTIONS "-Wall -Wextra -Wpedantic -Wno-unused-parameter ${SANITIZER_FLAGS}")

# Find Required Packages
find_package(Freetype REQUIRED)
find_package(PkgConfig REQUIRED)

# Find Wayland dependencies
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-server wayland-egl)
pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
pkg_check_modules(PAM REQUIRED pam)
pkg_check_modules(OPENGL REQUIRED gl)
pkg_check_modules(GLES REQUIRED glesv2)

# Ensure all required headers exist
set(TOML_HEADER "toml/toml.h")

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${TOML_HEADER}")
    message(FATAL_ERROR "** TOML header not found. Expected at: ${TOML_HEADER} **")
endif()

message(STATUS "Found TOML header: ${TOML_HEADER}")

# Include directories
include_directories(${FREETYPE_INCLUDE_DIRS} toml ${WAYLAND_INCLUDE_DIRS} ${XKBCOMMON_INCLUDE_DIRS} ${PAM_INCLUDE_DIRS})

# Add Executable
add_executable(${EXECUTABLE_NAME} src/main.c toml/toml.c)

# Link Libraries
target_link_libraries(${EXECUTABLE_NAME}
    PRIVATE ${FREETYPE_LIBRARIES}
    PRIVATE ${WAYLAND_LIBRARIES}
    PRIVATE ${XKBCOMMON_LIBRARIES}
    PRIVATE ${PAM_LIBRARIES}
    PRIVATE EGL GLESv2 m
)

# Apply Compiler Flags
target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic ${SANITIZER_FLAGS})
target_link_options(${EXECUTABLE_NAME} PRIVATE ${SANITIZER_FLAGS})

# Global Installation Support
option(BUILD_GLOBAL "Build globally and install" OFF)

# --- Handle Global Build ---------------------------------------------------------
if(DEFINED BUILD_GLOBAL AND BUILD_GLOBAL)
  set(CMAKE_BUILD_TYPE "Global")
  set(CMAKE_INSTALL_PREFIX "/usr/")
  message(STATUS "Starting GLOBAL_BUILD for Anvilock...")
  set(GLOBAL_SHADER_DIR "/usr/share/anvilock/shaders")

  install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
  install(DIRECTORY shaders/ DESTINATION ${GLOBAL_SHADER_DIR})  # Install shaders globally
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
  message(STATUS "Building locally, binaries will be in ./build/")

  set(LOCAL_SHADER_DIR "$ENV{HOME}/.local/share/anvilock/shaders")
  file(MAKE_DIRECTORY ${LOCAL_SHADER_DIR})
  file(COPY shaders/ DESTINATION ${LOCAL_SHADER_DIR})
endif()

# Ensure `config.toml` Exists
if (NOT EXISTS ${CONFIG_FILE})
    message(STATUS "Creating default config.toml...")
    file(MAKE_DIRECTORY ${CONFIG_DIR})
    file(WRITE ${CONFIG_FILE} 
        "[font]\nname = \"# your font name\"\npath = \"# your font path\"\n\n"
        "[bg]\nname = \"# your background name\"\npath = \"# your background path\"\n"
    )
endif()

include(CheckIncludeFiles)

set(STANDARD_HEADERS
    "assert.h"
    "ctype.h"
    "errno.h"
    "fcntl.h"
    "float.h"
    "inttypes.h"
    "limits.h"
    "math.h"
    "setjmp.h"
    "signal.h"
    "stdarg.h"
    "stddef.h"
    "stdio.h"
    "stdlib.h"
    "string.h"
    "time.h"
    "unistd.h"
    "stdbool.h"
)

message(STATUS "${BLUE}Checking Standard C Headers...${RESET}")
foreach(header ${STANDARD_HEADERS})
    check_include_files(${header} HAS_${header})
    if(HAS_${header})
      message(STATUS "${GREEN}✔ Found header: ${header}${RESET}")
    else()
      message(WARNING "${YELLOW}✘ Missing header: ${header}${RESET}")
    endif()
endforeach()

# Print Build Info
message(STATUS "\n\n-- Building anvilock v${PROJECT_VERSION} --")
message(STATUS "┌─ Configuration ────────────────────────")
message(STATUS "│ OS:                ${CMAKE_SYSTEM_NAME}")
message(STATUS "│ Build Type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "│ Build Generator:   ${CMAKE_GENERATOR}")
message(STATUS "│ Compiler:          ${CMAKE_C_COMPILER}")
message(STATUS "│ Compiler Flags:    ${COMPILE_OPTIONS}")
message(STATUS "│ Executable Name:   ${EXECUTABLE_NAME}")
message(STATUS "│ C Standard:        ${CMAKE_C_STANDARD}")
message(STATUS "│ Install Prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "│ Local Build Dir:   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "│ Freetype:          YES (${FREETYPE_LIBRARIES})")
message(STATUS "│ Wayland:           YES (${WAYLAND_LIBRARIES})")
message(STATUS "│ XKBCommon:         YES (${XKBCOMMON_LIBRARIES})")
message(STATUS "│ PAM:               YES (${PAM_LIBRARIES})")
message(STATUS "│ GLESv2:            YES (${GLES_LIBRARIES})")
message(STATUS "│ Config File:       ${CONFIG_FILE}")
message(STATUS "│ Global Install:    ${BUILD_GLOBAL}")
message(STATUS "│ Global shaders:    ${GLOBAL_SHADER_DIR}")
message(STATUS "│ Local shaders:     ${LOCAL_SHADER_DIR}")
message(STATUS "└───────────────────────────────────────\n")
