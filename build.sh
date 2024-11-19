#!/bin/bash

# Function to check dependencies using pkg-config
gum style --foreground 212 "Checking for required dependencies..."

REQUIRED_LIBS=(
  wayland-client
  wayland-server
  egl
  glesv2
  pam
  xkbcommon
  freetype2
)

missing_libs=()
for lib in "${REQUIRED_LIBS[@]}"; do
  if ! pkg-config --exists "$lib"; then
    gum style --foreground 196 "Dependency missing: $lib"
    missing_libs+=("$lib")
  else
    gum style --foreground 118 "Dependency found: $lib"
  fi
done

if [ ${#missing_libs[@]} -ne 0 ]; then
  gum style --foreground 196 "Error: Missing dependencies: ${missing_libs[*]}"
  exit 1
else
  gum style --foreground 118 "All dependencies are met."
fi

# Download stb_image.h
STB_URL="https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
STB_PATH="stb_image.h"

mkdir -p include
if [ ! -f "$STB_PATH" ]; then
  gum style --foreground 212 "Downloading stb_image.h..."
  if command -v wget &> /dev/null; then
    wget -q -O "$STB_PATH" "$STB_URL"
    gum style --foreground 118 "stb_image.h downloaded using wget."
  elif command -v curl &> /dev/null; then
    curl -s -o "$STB_PATH" "$STB_URL"
    gum style --foreground 118 "stb_image.h downloaded using curl."
  else
    gum style --foreground 196 "Error: wget or curl not found. Cannot download stb_image.h."
    exit 1
  fi
else
  gum style --foreground 118 "stb_image.h already exists. Skipping download."
fi

# Generate Wayland protocols
mkdir -p protocols/src

EXT_PROTOCOL="/usr/share/wayland-protocols/staging/ext-session-lock/ext-session-lock-v1.xml"
XDG_PROTOCOL="/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml"

if [ -f "$EXT_PROTOCOL" ]; then
  gum style --foreground 212 "Generating headers and sources for EXT_SESSION_LOCK_V1 protocol..."
  wayland-scanner client-header "$EXT_PROTOCOL" protocols/ext-session-lock-client-protocol.h
  wayland-scanner private-code "$EXT_PROTOCOL" protocols/src/ext-session-lock-client-protocol.c
  gum style --foreground 118 "EXT_SESSION_LOCK_V1 protocol generated successfully."
else
  gum style --foreground 196 "Error: EXT_SESSION_LOCK_V1 protocol file not found."
  exit 1
fi

if [ -f "$XDG_PROTOCOL" ]; then
  gum style --foreground 212 "Generating headers and sources for XDG_SHELL protocol..."
  wayland-scanner client-header "$XDG_PROTOCOL" protocols/xdg-shell-client-protocol.h
  wayland-scanner private-code "$XDG_PROTOCOL" protocols/src/xdg-shell-client-protocol.c
  gum style --foreground 118 "XDG_SHELL protocol generated successfully."
else
  gum style --foreground 196 "Error: XDG_SHELL protocol file not found."
  exit 1
fi

# Ask user for build system choice
gum style --foreground 212 "Select build system:"
build_system=$(gum choose "Make" "CMake" "Meson")

case "$build_system" in
  "Make")
    gum style --foreground 212 "Building project with Make..."
    if make; then
      gum style --foreground 118 "Build completed successfully with Make."
    else
      gum style --foreground 196 "Build failed with Make."
      exit 1
    fi
    ;;
  "CMake")
    gum style --foreground 212 "Building project with CMake..."
    mkdir -p build && cd build
    if cmake .. && cmake --build .; then
      gum style --foreground 118 "Build completed successfully with CMake."
    else
      gum style --foreground 196 "Build failed with CMake."
      exit 1
    fi
    cd ..
    ;;
  "Meson")
    gum style --foreground 212 "Building project with Meson..."
    meson setup build
    if meson compile -C build; then
      gum style --foreground 118 "Build completed successfully with Meson."
    else
      gum style --foreground 196 "Build failed with Meson."
      exit 1
    fi
    ;;
  *)
    gum style --foreground 196 "Error: Invalid build system selected."
    exit 1
    ;;
esac

gum style --foreground 118 "Script completed successfully."
