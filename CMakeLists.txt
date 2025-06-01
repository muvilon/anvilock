cmake_minimum_required(VERSION 3.25)
include(cmake/CheckHeaders.cmake)

project(anvilock VERSION 1.0 LANGUAGES C CXX)

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
    set(CMAKE_CXX_COMPILER clang++ CACHE STRING "C++ Compiler" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

string(ASCII 27 ESC)
set(RESET "${ESC}[0m")
set(GREEN "${ESC}[32m")
set(YELLOW "${ESC}[33m")
set(BLUE   "${ESC}[34m")

# Define paths
set(CONFIG_DIR "$ENV{HOME}/.config/anvilock")
set(CONFIG_FILE "${CONFIG_DIR}/config.toml")

# Sanitizer Flags
set(SANITIZER_FLAGS "")
if(CMAKE_BUILD_TYPE STREQUAL "Debug-ASan")
    set(EXECUTABLE_NAME "anvilock-DBG-ASan")
    set(SANITIZER_FLAGS -fsanitize=address -fsanitize=undefined -g)
    message(STATUS "Enabling AddressSanitizer")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug-TSan")
    set(EXECUTABLE_NAME "anvilock-DBG-TSan")
    set(SANITIZER_FLAGS -fsanitize=thread -fsanitize=undefined -g)
    message(STATUS "Enabling ThreadSanitizer")
else()
    set(CMAKE_BUILD_TYPE "Release")
    set(EXECUTABLE_NAME "anvilock")
    message(STATUS "Skipping Sanitizers for Release build")
endif()

# Strict Warning Flags
set(STRICT_WARNINGS
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
)

set(COMPILE_OPTIONS ${STRICT_WARNINGS} ${SANITIZER_FLAGS})

check_headers(STANDARD_C_HEADERS "C")
check_headers(STANDARD_CXX_HEADERS "CXX")
check_headers(SYSTEM_HEADERS "C")

# Find Required Packages
find_package(Freetype REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-server wayland-egl)
pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)
pkg_check_modules(PAM REQUIRED pam)
pkg_check_modules(OPENGL REQUIRED gl)
pkg_check_modules(GLES REQUIRED glesv2)

include_directories(${FREETYPE_INCLUDE_DIRS} toml ${WAYLAND_INCLUDE_DIRS} ${XKBCOMMON_INCLUDE_DIRS} ${PAM_INCLUDE_DIRS})

set(PROTOCOL_SRC_FILES 
  protocols/src/xdg-shell-client-protocol.c 
  protocols/src/ext-session-lock-client-protocol.c
)

add_library(anvilock_logger STATIC ${CMAKE_SOURCE_DIR}/src/logger/Log.cpp)
target_include_directories(anvilock_logger PUBLIC ${CMAKE_SOURCE_DIR}/../)
target_compile_options(anvilock_logger PRIVATE ${COMPILE_OPTIONS})

set(SRC_FILES
  ${CMAKE_SOURCE_DIR}/src/pam/PamAuthenticator.cpp
  ${CMAKE_SOURCE_DIR}/src/toml/Parser.cpp
  ${CMAKE_SOURCE_DIR}/src/global/ClientState.cpp
  ${CMAKE_SOURCE_DIR}/src/wayland/RegistryHandler.cpp
  ${CMAKE_SOURCE_DIR}/src/wayland/OutputHandler.cpp
  ${CMAKE_SOURCE_DIR}/src/wayland/xdg/WmBaseHandler.cpp
  ${CMAKE_SOURCE_DIR}/src/wayland/xdg/SurfaceHandler.cpp
  ${CMAKE_SOURCE_DIR}/src/wayland/seats/SeatHandler.cpp
  ${CMAKE_SOURCE_DIR}/src/wayland/session-lock/SessionLockHandler.cpp
  ${CMAKE_SOURCE_DIR}/src/wayland/seats/PointerHandler.cpp
  ${CMAKE_SOURCE_DIR}/src/wayland/seats/KeyboardHandler.cpp
  ${CMAKE_SOURCE_DIR}/src/config/ConfigHandler.cpp
  ${CMAKE_SOURCE_DIR}/src/shaders/ShaderManager.cpp
  ${CMAKE_SOURCE_DIR}/src/renderer/EGL.cpp
  ${CMAKE_SOURCE_DIR}/src/widgets/helpers/WidgetHelper.cpp
  ${CMAKE_SOURCE_DIR}/src/widgets/BackgroundTexture.cpp
  ${CMAKE_SOURCE_DIR}/src/widgets/PasswordField.cpp
  ${CMAKE_SOURCE_DIR}/src/widgets/TimeBox.cpp
  ${CMAKE_SOURCE_DIR}/src/widgets/WidgetInterface.cpp
)

add_executable(${EXECUTABLE_NAME} src/main.cpp ${SRC_FILES} ${PROTOCOL_SRC_FILES})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../)
target_link_libraries(${EXECUTABLE_NAME}
    PRIVATE ${FREETYPE_LIBRARIES}
    PRIVATE ${WAYLAND_LIBRARIES}
    PRIVATE ${XKBCOMMON_LIBRARIES}
    PRIVATE ${PAM_LIBRARIES}
    PRIVATE EGL GLESv2 m
    PRIVATE anvilock_logger
)
target_compile_options(${EXECUTABLE_NAME} PRIVATE ${COMPILE_OPTIONS})
target_link_options(${EXECUTABLE_NAME} PRIVATE ${SANITIZER_FLAGS})

# Global Installation Support
if(DEFINED BUILD_GLOBAL AND BUILD_GLOBAL)
  set(CMAKE_BUILD_TYPE "Global")
  set(CMAKE_INSTALL_PREFIX "/usr/")
  message(STATUS "Starting GLOBAL_BUILD for Anvilock...")
  set(GLOBAL_SHADER_DIR "/usr/share/anvilock/shaders")

  install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
  install(DIRECTORY shaders/ DESTINATION ${GLOBAL_SHADER_DIR})
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  message(STATUS "Building locally, binaries will be in ./build/")

  set(LOCAL_SHADER_DIR "$ENV{HOME}/.local/share/anvilock/shaders")
  file(MAKE_DIRECTORY ${LOCAL_SHADER_DIR})
  file(COPY ${CMAKE_SOURCE_DIR}/shaders/ DESTINATION ${LOCAL_SHADER_DIR})
endif()

if(NOT EXISTS ${CONFIG_FILE})
    message(STATUS "Config not found, copying default config...")
    file(MAKE_DIRECTORY ${CONFIG_DIR})
    file(COPY ${DEFAULT_CONFIG} DESTINATION ${CONFIG_DIR})
endif()

add_subdirectory(examples/toml)

# -- Print Build Info ---------------------------------------------------------
message(STATUS "\n\n-- Building anvilock v${PROJECT_VERSION} --")
message(STATUS "┌─ Configuration ────────────────────────")
message(STATUS "│ OS:                ${CMAKE_SYSTEM_NAME}")
message(STATUS "│ Build Type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "│ Build Generator:   ${CMAKE_GENERATOR}")
message(STATUS "│ Compiler:          ${CMAKE_C_COMPILER}")
message(STATUS "│ Compiler Flags:    ${COMPILE_OPTIONS}")
message(STATUS "│ Executable Name:   ${EXECUTABLE_NAME}")
message(STATUS "│ C++ Standard:      ${CMAKE_CXX_STANDARD}")
message(STATUS "│ Install Prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "│ Local Build Dir:   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "│ Freetype:          ${FREETYPE_LIBRARIES}")
message(STATUS "│ Wayland:           ${WAYLAND_LIBRARIES}")
message(STATUS "│ XKBCommon:         ${XKBCOMMON_LIBRARIES}")
message(STATUS "│ PAM:               ${PAM_LIBRARIES}")
message(STATUS "│ GLESv2:            ${GLES_LIBRARIES}")
message(STATUS "│ Config File:       ${CONFIG_FILE}")
message(STATUS "│ Global Install:    ${BUILD_GLOBAL}")
message(STATUS "│ Global shaders:    ${GLOBAL_SHADER_DIR}")
message(STATUS "│ Local shaders:     ${LOCAL_SHADER_DIR}")
message(STATUS "└───────────────────────────────────────\n")
