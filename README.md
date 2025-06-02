# Anvilock (Unstable branch)

This branch aims to port Anvilock to a better and readable codebase using modern C++ (ideally cpp version >= 20)

> [!IMPORTANT]
> 
> Unstable branch as the name suggests, is VERY unstable!
> 
> Expect breaking changes with each passing commit.
> 

## Dependencies

Currently you will have to manually configure these deps in your system (which is really not *that* hard) but in the future will have a way to ensure deps install before attempted compilation of Anvilock.

### Runtime Dependencies

Here’s your updated table, replacing **`glesv2`** with **`gles3`** across all columns and package names:

| Library             | Purpose                                            | Arch Linux Package   | Ubuntu Package              | Fedora Package        |
| ------------------- | -------------------------------------------------- | -------------------- | --------------------------- | --------------------- |
| `libwayland-client` | Handles Wayland client communication.              | `wayland`            | `libwayland-client0`        | `wayland`             |
| `libwayland-server` | Enables communication with the display server.     | `wayland`            | `libwayland-server0`        | `wayland`             |
| `libwayland-egl`    | Interfaces with OpenGL ES for Wayland integration. | `mesa`               | `libwayland-egl1`           | `mesa-libwayland-egl` |
| `gles3`             | Efficient 2D/3D rendering using OpenGL ES 3.0+.    | `mesa` or `libglvnd` | `libgles-dev` or `libgles3` | `mesa-libGLES`        |
| `freetype2`         | *(Deprecated)* Previously used for text rendering. | `freetype2`          | `libfreetype6`              | `freetype`            |
| `libxkbcommon`      | Keyboard keymap handling and layout management.    | `libxkbcommon`       | `libxkbcommon0`             | `libxkbcommon`        |
| `libpam`            | PAM authentication framework integration.          | `pam`                | `libpam0g`                  | `pam`                 |
| `libc`              | Standard C library for memory handling and I/O.    | `glibc`              | `libc6`                     | `glibc`               |
| `libm`              | Math operations (e.g., used by `stb_image.h`).     | `glibc` (included)   | `libm.so.6` (in `libc6`)    | `glibc` (included)    |

> [!NOTE]
>
> * On **Ubuntu**, `libgles-dev` provides development headers for both GLES2 and GLES3.
> * On **Arch**, `mesa` and/or `libglvnd` provide `libGLESv2.so` which includes GLES3.
> * On **Fedora**, `mesa-libGLES` supports GLES 3+ with no separate `gles3` package.
> 

### Build Dependencies

| Tool         | Role                                     | Arch Linux Package | Ubuntu Package | Fedora Package       |
| ------------ | ---------------------------------------- | ------------------ | -------------- | -------------------- |
| `pkg-config` | Finds and configures required libraries. | `pkgconf`          | `pkg-config`   | `pkgconf-pkg-config` |
| `CMake`      | Main Build system.                       | `cmake`            | `cmake`        | `cmake`              |
| `Make`       | Required alongside CMake.                | `make`             | `make`         | `make`               |
| `gcc`        | C/C++ compiler.                          | `gcc`              | `gcc`          | `gcc`                |
| `clang`      | Alternative C/C++ compiler.              | `clang`            | `clang`        | `clang`              |


## Building

Firstly clone this branch recursively:

```bash 
git clone --branch unstable --recursive https://github.com/muvilon/Anvilock.git
```

Then build:

```bash 
cmake -S . -B build 
cmake --build build/
```

### Setting log level

You can simply set the log level using `ANVLK_LOG_LEVEL` environment variable in the terminal.

The environment variable can be set to:

1. **ERROR**
2. **WARN**
3. **INFO**
4. **DEBUG**
5. **TRACE**

These levels are listed in priority and upon choosing a log level (say `X`), it will print logs of priority `X` **AND** above

```bash 
export ANVLK_LOG_LEVEL=INFO
./build/anvilock 

# For future changes 
ANVLK_LOG_LEVEL=debug ./build/anvilock
```

## FAQ

1. Will this work for KDE or GNOME?

Short Answer is **NO**. They have their own implementations of the Wayland Protocol so unless we attempt to abide by both the general **ext-session-lock-v1** and their protocols, it will **NOT WORK** on those DEs

2. Where can I use this?

Any WM/DE that does not have custom implementations of the Wayland Protocol and has the **ext-session-lock-v1** protocol

Few examples: Hyprland, Sway, Wayfire, Cosmic, etc.

> [!NOTE]
> 
> This will **NOT WORK** for compositors like Weston which LACK the **ext-session-lock-v1** protocol needed for Anvilock to work.
> 

More docs on the porting process will be out soon.
