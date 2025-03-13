# ANVILOCK

ANVILOCK stands for **A Naive yet Intelligent LOCK**

An upcoming screen lock for Wayland compositors that abides by the **ext-session-lock-v1** Wayland Protocol.

> [!IMPORTANT]
>
> ANVILOCK is a [WIP] - Work in Progress
>
> **This project is undergoing a massive revamp!!**
> 
> Stay tuned for more updates!

## Understanding Anvilock 

To further understand how we created a Wayland based screen locker from scratch, check out this **[blog](https://s1dd.is-a.dev/blog/anvilock-blog)**

## List of Contents

1. [Project Structure](https://github.com/muvilon/anvilock?tab=readme-ov-file#1-project-structure)
2. [Dependencies](https://github.com/muvilon/anvilock?tab=readme-ov-file#2-dependencies)
3. [Building](https://github.com/muvilon/anvilock?tab=readme-ov-file#3-building)
4. [Config](https://github.com/muvilon/anvilock?tab=readme-ov-file#4-configuration)
5. [Future](https://github.com/muvilon/anvilock?tab=readme-ov-file#5-future)

## 1. Project Structure

```
ANVILOCK
├── CHANGELOG/
├── LICENSE
├── README.md
├── SECURITY.md
├── VERSION
├── include/
│   ├── config/
│   │   ├── config.h
│   ├── deprecated/
│   │   ├── surface_colors.h
│   │   ├── unicode.h
│   │   └── NOTE.md
│   ├── freetype/
│   │   └── freetype.h
│   ├── graphics/
│   │   ├── egl.h
│   │   └── shaders.h
│   ├── memory/
│   │   └── anvil_mem.h
│   ├── pam/
│   │   ├── pam.h
│   │   └── password_buffer.h
│   ├── wayland/
│   │   ├── session_lock_handle.h
│   │   ├── shared_mem_handle.h
│   │   ├── wl_buffer_handle.h
│   │   ├── wl_keyboard_handle.h
│   │   ├── wl_output_handle.h
│   │   ├── wl_pointer_handle.h
│   │   ├── wl_registry_handle.h
│   │   ├── wl_seat_handle.h
│   │   ├── xdg_surface_handle.h
│   │   └── xdg_wm_base_handle.h
│   ├── global_funcs.h 
│   ├── client_state.h 
│   ├── log.h 
│   └── NOTE.md 
├── src/
│   ├── main.c 
│   └── main.h
├── toml
│   ├── toml.h
│   └── toml.c
├── shaders/
├── Makefile
├── meson.build
├── CMakeLists.txt
├── stb_image.h
└── protocols
    ├── xdg-shell-client-protocol.h
    ├── ext-session-lock-client-protocol.h
    └── src
        ├── xdg-shell-client-protocol.c
        └── ext-session-lock-client-protocol.c
```

#### Protocol Files:

> [!IMPORTANT]
> 
> For more information regarding Wayland Protocol generation and its working,
> 
> Read [PROTOCOLS.md](https://github.com/muvilon/anvilock/blob/main/protocols/PROTOCOLS.md)
> 
> It gives a better understanding of generating protocol headers and source code along with details of what protocols to abide by to compile anvilock.
> 

- **protocols/xdg-shell-client-protocol.h**: Protocol definitions for XDG shell clients, managing surfaces and windows.
- **protocols/src/xdg-shell-client-protocol.c**: Implementation of the XDG shell client protocol.

- **protocols/ext-session-lock-client-protocol.h**: Protocol definitions for `ext-session-lock-v1` protocol, session lock listener and management of lock surface.
- **protocols/src/ext-session-lock-client-protocol.c**: Implementation of `ext-session-lock-v1` protocol.

#### Toml Files:

- **toml/toml.h**: Header file for tomlc99 parser.
- **toml/toml.c**: Implementation of tomlc99 parser.

The TOML Parser being used in this repo is thanks to [tomlc99](https://github.com/cktan/tomlc99)

#### stb Files:

Headers used to draw and write images which are then loaded into an EGL texture for rendering 

`stb_image.h` and `stb_image_write.h` are thanks to [stb](https://github.com/nothings/stb)

> [!NOTE]
> 
> You can either you `build.sh` to install stb_image.h:
> ```bash 
> ./build.sh
> ```
> 
> Or if you want, you can use **Makefile** to do the same:
> 
> ```bash 
> make init # to just initialize the repository (does NOT build)
> make release # runs make init and make protocols before building with apt file checking
> ```
> 

## 2. Dependencies

ANVILOCK relies on the following dependencies:

- **libwayland-client**: Handles Wayland client communication.
- **libwayland-server**: Communication with the display server is possible through this library.
- **libwayland-egl**: An interfacing library with OpenEGL ES 2.0 API binding for Wayland.
- **glesv2**: Used for rendering 2D and 3D graphics on embedded systems and low-power devices, ensuring efficient graphics performance.
- **freetype2**: Facilitates high-quality text rendering by processing and rasterizing font files in applications. (NOT BEING USED CURRENTLY)
- **libxkbcommon**: Provides keyboard keymap compilation and handling.
- **libpam**: Integration with the PAM authentication framework.
- **libc**: Standard C library for memory handling, I/O, and other essential operations.
- **libm**: Mathematical operations required by `stb_image.h` header.

### Build dependencies:

- **pkg-config**: To find and locate the required libraries.
- **CMake** / **Make** / **Meson**: Any one is enough to locate and compile anvilock with default configurations.
- **gcc** / **clang**: Any one compiler is enough to compile the source code.

All dependencies must be installed on your system before building.

## Building

### Prerequisites

Ensure that the dependencies listed in [Dependencies](https://github.com/muvilon/anvilock?tab=readme-ov-file#2-dependencies) are installed.

## Build Instructions:

Simply run this in your terminal:

```bash
./build.sh
```

The build script will take care of dependencies list, loading `stb_image.h` from github and loading the required wayland protocols for anvilock to run

## Build Instructions (Manual)

### 1. Cloning the Repository

First, clone the repository:

```bash
git clone https://github.com/muvilon/anvilock.git
```

### 2. Building the Application

You can build the application using one of the following methods: Make (using CMake), CMake, or Meson. Choose one based on your preference or environment.

> [!IMPORTANT]
> 
> It is to be noted that building Anvilock using `Makefile` is the most advised 
> way of using the build system.
> 
> Other ways may work but they are outdated (OR) are undergoing an overhaul.
> 

#### Method 1: Building with Make

1. Run this command to build the **RELEASE** version of Anvilock:

    ```bash 
    make release
    ``` 

There are a **LOT** of targets that can be used using Makefile! Here is a comprehensive list of them all that will make you life much easier:

**MAKEFILE TARGETS:**

### Build Targets  

#### `all` (Default)  
Runs the `release` target to compile the project in **Release mode**.  

#### `release`  
- Runs `init` (downloads `stb_image.h` if needed).  
- Creates the `build/` directory.  
- Configures the project using `CMake` with `-DCMAKE_BUILD_TYPE=Release`.  
- Compiles the project using `make`.  

#### `debug`  
- Creates the `build/` directory.  
- Configures the project for **Debug mode** (`-DCMAKE_BUILD_TYPE=Debug`).  
- Compiles the project using `make`.  

#### `asan`  
- Creates `build-asan/` for AddressSanitizer builds.  
- Configures the project with `-DCMAKE_BUILD_TYPE=Debug-ASan`.  
- Compiles the project.  

#### `tsan`  
- Creates `build-tsan/` for ThreadSanitizer builds.  
- Configures the project with `-DCMAKE_BUILD_TYPE=Debug-TSan`.  
- Compiles the project.  

### Protocol Generation  

#### `protocols`  
Generates headers and source files for Wayland protocols:  
- `ext-session-lock-client-protocol.h` and `ext-session-lock-client-protocol.c`.  
- `xdg-shell-client-protocol.h` and `xdg-shell-client-protocol.c`.  

It uses `wayland-scanner` to generate these files from the system protocol XML files.  

### Initialization

#### `init`  
- Downloads `stb_image.h` if it does not exist.  
- Runs `make protocols` to generate Wayland protocol files.

### Code Quality  

#### `format`  
Runs `clang-format` on all `.c` and `.h` files in `src/` and `include/`.  

#### `tidy`  
Runs `clang-tidy` on all `.c` and `.h` files in `src/` and `include/`.

### Cleanup  

#### `clean`  
Removes `build/`, `build-asan/`, and `build-tsan/` directories.

### Installation  

#### `install`  
Installs the compiled binaries and resources using `make install` from the build directory.  

#### `uninstall`  
Reads `install_manifest.txt` and removes installed files.  

### Running  

#### `run`  
Runs the compiled executable from the `build/` directory.

#### Method 2: Building with CMake

1. Navigate to the project directory:

   ```bash
   cd anvilock
   ```

2. Create a build directory and navigate into it:

   ```bash
   mkdir build && cd build
   ```

3. Configure the project:

   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release # build type can be release, debug, Debug-ASan or Debug-TSan
   # You can add other flags here as you wish, refer to CMakeLists.txt for more build options!
   ```

4. Build the application:

   ```bash
   make
   ```

5. Modify the `~/.config/anvilock/config.toml` file to use your font

6. Run the application:

   ```bash
   ./anvilock
   ```

**Caveats for CMake:**

- Ensure that you have the necessary CMake version (3.22 or later).
- You **NEED** to install `cmake` and `pkg-config` if they are not already available.

#### Method 3: Building with Meson

1. Navigate to the project directory:

   ```bash
   cd anvilock
   ```

2. Create a build directory and set it up with Meson:

   ```bash
   meson setup build
   ```

3. Build the application with or without the placeholder config file:

   ```bash
   meson compile -C build [config]
   ```

4. Modify the `~/.config/anvilock/config.toml` file to use your font

5. Run the application:

   ```bash
   ./build/anvilock
   ```

**Caveats for Meson:**

- Ensure that you have Meson installed (you can usually install it via your package manager).
- Meson defaults to an out-of-source build, which helps keep the project directory clean.
- If you encounter issues with dependencies, you may need to install `ninja` (the default backend for Meson).

## 4. Configuration  

All configurations are done through a `config.toml` file that will be generated in `~/.config/anvilock/` if not present.  

Make sure to check out `~/.config/anvilock/config.toml` for more!  

> [!NOTE]  
> More configurations and customizations are to come!  
>  
> An example config file is explained in [config.toml](https://github.com/muvilon/anvilock/blob/main/assets/examples/config.toml).  

### Available Configuration Fields  

#### `[font]`  
Configures the font used for text rendering on the lock screen.  
- `name` – A custom name for the font (optional).  
- `path` – Absolute path to the font file (e.g., `.ttf`, `.otf`).  

#### `[bg]`  
Configures the background image for the lock screen.  
- `name` – A custom name for the background (optional).  
- `path` – Absolute path to the image file.  

#### `[debug]`  
Controls debug logging.  
- `debug_log_enable` – Enables (`"true"`) or disables (`"false"`) detailed logging for pointers, keyboards, shaders, and other interfaces.  

#### `[time]`  
Controls the time format displayed on the lock screen.  
- `time_format` – Defines the format of the clock display. Options:  
  - `"H:M"` → Hours & Minutes  
  - `"H:M:S"` → Hours, Minutes & Seconds  

#### `[time_box]`  
Defines the position of the time display on the screen. Each key specifies a corner of the time box in **normalized coordinates** (where `-1.0` is the left/bottom and `1.0` is the right/top).  
- `top_left` – Coordinates of the top-left corner.  
- `top_right` – Coordinates of the top-right corner.  
- `bottom_left` – Coordinates of the bottom-left corner.  
- `bottom_right` – Coordinates of the bottom-right corner.

### Conclusion

This project is far from over and has very active **BREAKING** changes so build and use with caution.

#### FAQ

1. Will this work for KDE or GNOME? 

Short Answer is NO. They have their own implementations of the Wayland Protocol so unless we attempt to abide by both the general `ext-session-lock-v1` and their protocols, it will NOT work on those DEs 

2. Where can I use this?

Any WM/DE that does not have custom implementations of the Wayland Protocol and has the `ext-session-lock-v1` protocol 

Few examples: Hyprland, Sway, Wayfire, Cosmic, etc. 

-> This will not work for compositors like Weston which lack the `ext-session-lock-v1` protocol needed for anvilock to work.

## 5. Future

Stay tuned for more updates as we progress through development.

Anvilock is an open source software licensed under the [BSD 3 Clause](https://github.com/muvilon/anvilock/blob/main/LICENSE) License.
