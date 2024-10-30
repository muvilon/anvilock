# ANVILOCK

ANVILOCK stands for **A Naive yet Intelligent LOCK**

An upcoming screen lock for Wayland compositors that abides by the **ext-session-lock-v1** Wayland Protocol.

> [!IMPORTANT]
>
> ANVILOCK is a [WIP] - Work in Progress
>
> Stay tuned for more updates!

---

## List of Contents

1. [Project Structure](https://github.com/muvilon/anvilock?tab=readme-ov-file#1-project-structure)
2. [Dependencies](https://github.com/muvilon/anvilock?tab=readme-ov-file#2-dependencies)
3. [Building](https://github.com/muvilon/anvilock?tab=readme-ov-file#3-building)
4. [Config](https://github.com/muvilon/anvilock?tab=readme-ov-file#4-configuration)
4. [Future](https://github.com/muvilon/anvilock?tab=readme-ov-file#5-future)

---

## 1. Project Structure

```
ANVILOCK
├── main.c
├── client_state.h
├── log.h
├── password_buffer.h
├── pam.h
├── wl_registry_handle.h
├── wl_buffer_handle.h
├── wl_seat_handle.h
├── surface_colors.h
├── wl_keyboard_handle.h
├── wl_pointer_handle.h
├── xdg_surface_handle.h
├── xdg_wm_base_handle.h
├── shared_mem_handle.h
├── toml
├   ├── toml.h
├   └── toml.c
└── protocols
    ├── xdg-shell-client-protocol.h
    ├── ext-session-lock-client-protocol.h
    └── src
        ├── xdg-shell-client-protocol.c
        └── ext-session-lock-client-protocol.c
```

### File Descriptions

- **client_state.h**: Handles the overall state of the client, including tracking of surfaces, input, and buffer states.
- **log.h**: Centralized logging mechanism to track operations and errors.
- **password_buffer.h**: Secure handling of password storage, using memory sanitization techniques.
- **pam.h**: Integration with Pluggable Authentication Modules (PAM) for user authentication.
- **wl_registry_handle.h**: Handles Wayland registry interface, responsible for connecting to the Wayland compositor.
- **wl_buffer_handle.h**: Deals with buffer management for rendering lock screens.
- **wl_seat_handle.h**: Handles Wayland seat interface, used to manage input devices like keyboards and pointers.
- **wl_keyboard_handle.h**: Manages keyboard input and event handling.
- **wl_pointer_handle.h**: Manages pointer input and event handling.
- **xdg_surface_handle.h**: Handles XDG surface protocols to create and manage window surfaces.
- **xdg_wm_base_handle.h**: Deals with the XDG window manager base protocol for interacting with compositor windowing systems.
- **shared_mem_handle.h**: Manages shared memory to allow efficient communication between processes.

#### Protocol Files:

- **protocols/xdg-shell-client-protocol.h**: Protocol definitions for XDG shell clients, managing surfaces and windows.
- **protocols/src/xdg-shell-client-protocol.c**: Implementation of the XDG shell client protocol.

- **protocols/ext-session-lock-client-protocol.h**: Protocol definitions for `ext-session-lock-v1` protocol, session lock listener and management of lock surface.
- **protocols/src/ext-session-lock-client-protocol.c**: Implementation of `ext-session-lock-v1` protocol.

#### Toml Files:

- **toml/toml.h**: Header file for tomlc99 parser.
- **toml/toml.c**: Implementation of tomlc99 parser.

The TOML Parser being used in this repo is thanks to [tomlc99](https://github.com/cktan/tomlc99)

---

## 2. Dependencies

ANVILOCK relies on the following dependencies:

- **libwayland-client**: Handles Wayland client communication.
- **libxkbcommon**: Provides keyboard keymap compilation and handling.
- **libpam**: Integration with the PAM authentication framework.
- **libc**: Standard C library for memory handling, I/O, and other essential operations.

All dependencies must be installed on your system before building.

---

## Building

### Prerequisites

Ensure that the following are installed:

- **Wayland development libraries**
- **PAM development libraries**
- **CMake** (if building with CMake) or **Meson** (if building with Meson)
- **gcc** or **clang** for compiling the project

## Build Instructions

### 1. Cloning the Repository

First, clone the repository:

```bash
git clone https://github.com/muvilon/anvilock.git
```

### 2. Building the Application

You can build the application using one of the following methods: Make, CMake, or Meson. Choose one based on your preference or environment.

#### Method 1: Building with Make

1. Navigate to the project directory:

   ```bash
   cd anvilock
   ```

2. Build the application:

   ```bash
   make
   ```

3. Modify the config.toml file to use your font

4. Run the application (ensure the compositor is Wayland-compatible):

   ```bash
   ./anvilock
   ```

**Caveats for Make:**

- Ensure that the `Makefile` is configured properly to find all necessary libraries and dependencies.
- The Make build may not automatically handle out-of-source builds, so you might want to keep your source directory clean.

---

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
   cmake ..
   ```

4. Build the application:

   ```bash
   make
   ```

5. Modify the config.toml file to use your font

6. Run the application:

   ```bash
   ./anvilock
   ```

**Caveats for CMake:**

- Ensure that you have the necessary CMake version (3.10 or later).
- You may need to install `cmake` and `pkg-config` if they are not already available.
- CMake supports out-of-source builds, keeping the source directory clean.

---

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

4. Modify the config.toml file to use your font

5. Run the application:

   ```bash
   ./build/anvilock
   ```

**Caveats for Meson:**

- Ensure that you have Meson installed (you can usually install it via your package manager).
- Meson defaults to an out-of-source build, which helps keep the project directory clean.
- If you encounter issues with dependencies, you may need to install `ninja` (the default backend for Meson).

---

## 4. Configuration

All configurations are done through a `config.toml` file that will be generated if not present

Currently the only aspect which is configurable is the **font** that is rendered by the **freetype2** package onto the lock surface

As we progress and move from basic shell rendering to something more graphical like EGL, we will have more configurations available

### Conclusion

Choose the build system that best fits your needs. Each method has its own advantages and potential pitfalls, so feel free to reach out if you encounter any issues or need further assistance!

---

## 5. Future

Stay tuned for more updates as we progress through development.

Anvilock is an open source software licensed under the [BSD 3 Clause](https://github.com/muvilon/anvilock/blob/main/LICENSE) License.

---
