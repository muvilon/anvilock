#!/bin/bash

# Function to install missing dependencies on Fedora
install_dependencies() {
  missing_deps=()
  for dep in "$@"; do
    if ! command -v "$dep" &> /dev/null; then
      missing_deps+=("$dep")
    fi
  done

  if [ ${#missing_deps[@]} -ne 0 ]; then
    echo "Installing missing dependencies: ${missing_deps[*]}..."
    sudo dnf install -y "${missing_deps[@]}"
  fi
}

# Function to install specific libraries
install_missing_libs() {
  sudo dnf install -y wayland-devel libxkbcommon-devel pam-devel mesa-libGLES-devel freetype-devel
}

# Install dependencies
install_dependencies curl wget pkg-config wayland-protocols-devel wayland-scanner
echo "[FEDORA SCRIPT] Downloaded dependencies successfully"

# Install additional libraries if missing
install_missing_libs

# Function to check pkg-config dependencies
echo "Checking for required pkg-config dependencies..."

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
    echo "Dependency missing: $lib"
    missing_libs+=("$lib")
  else
    echo "Dependency found: $lib"
  fi
done

if [ ${#missing_libs[@]} -ne 0 ]; then
  echo "Error: Missing dependencies: ${missing_libs[*]}. Trying to install..."
  install_missing_libs
fi

echo "All dependencies are met."

# Download stb_image.h
STB_URL="https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
STB_PATH="stb_image.h"

mkdir -p include
if [ ! -f "$STB_PATH" ]; then
  echo "Downloading stb_image.h..."
  if command -v wget &> /dev/null; then
    wget -q -O "$STB_PATH" "$STB_URL"
    mv "$STB_PATH" ../..
    echo "stb_image.h downloaded using wget."
  elif command -v curl &> /dev/null; then
    curl -s -o "$STB_PATH" "$STB_URL"
    echo "stb_image.h downloaded using curl."
  else
    echo "Error: wget or curl not found. Cannot download stb_image.h."
    exit 1
  fi
else
  echo "stb_image.h already exists. Skipping download."
fi

# Generate Wayland protocols
mkdir -p protocols/src

EXT_PROTOCOL="/usr/share/wayland-protocols/staging/ext-session-lock/ext-session-lock-v1.xml"
XDG_PROTOCOL="/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml"

if [ -f "$EXT_PROTOCOL" ]; then
  echo "Generating headers and sources for EXT_SESSION_LOCK_V1 protocol..."
  wayland-scanner client-header "$EXT_PROTOCOL" protocols/ext-session-lock-client-protocol.h
  wayland-scanner private-code "$EXT_PROTOCOL" protocols/src/ext-session-lock-client-protocol.c
  echo "EXT_SESSION_LOCK_V1 protocol generated successfully."
else
  echo "Error: EXT_SESSION_LOCK_V1 protocol file not found."
  exit 1
fi

if [ -f "$XDG_PROTOCOL" ]; then
  echo "Generating headers and sources for XDG_SHELL protocol..."
  wayland-scanner client-header "$XDG_PROTOCOL" protocols/xdg-shell-client-protocol.h
  wayland-scanner private-code "$XDG_PROTOCOL" protocols/src/xdg-shell-client-protocol.c
  echo "XDG_SHELL protocol generated successfully."
else
  echo "Error: XDG_SHELL protocol file not found."
  exit 1
fi

# Select build system (automatically use Make for simplicity in CI)
echo "Selecting build system: Make"
build_system="Make"

case "$build_system" in
  "Make")
    echo "Building project with Make..."
    if make; then
      echo "Build completed successfully with Make."
    else
      echo "Build failed with Make."
      exit 1
    fi
    ;;
  "CMake")
    echo "Building project with CMake..."
    mkdir -p build && cd build
    if cmake .. && cmake --build .; then
      echo "Build completed successfully with CMake."
    else
      echo "Build failed with CMake."
      exit 1
    fi
    cd ..
    ;;
  "Meson")
    echo "Building project with Meson..."
    meson setup build
    if meson compile -C build; then
      echo "Build completed successfully with Meson."
    else
      echo "Build failed with Meson."
      exit 1
    fi
    ;;
  *)
    echo "Error: Invalid build system selected."
    exit 1
    ;;
esac

echo "Script completed successfully."
