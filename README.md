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
4. [Future](https://github.com/muvilon/anvilock?tab=readme-ov-file#4-future)

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
└── protocols
    ├── xdg-shell-client-protocol.h
    ├── ext-session-lock-client-protocol.h
    └── src
        └── xdg-shell-client-protocol.c
        ├── ext-session-lock-client-protocol.c
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

---

## 2. Dependencies

ANVILOCK relies on the following dependencies:

- **libwayland-client**: Handles Wayland client communication.
- **libxkbcommon**: Provides keyboard keymap compilation and handling.
- **libpam**: Integration with the PAM authentication framework.
- **libc**: Standard C library for memory handling, I/O, and other essential operations.

All dependencies must be installed on your system before building.

---

## 3. Building

### Prerequisites:

Ensure that the following are installed:

- **Wayland development libraries**
- **PAM development libraries**
- **CMake or Meson** (depending on the project’s build system)
- **gcc/clang** for compiling the project

### Build Instructions:

1. Clone the repository:
   ```bash
   git clone https://github.com/muvilon/anvilock.git
   ```

2. Navigate to the project directory and build the application:
   ```bash
   cd anvilock
   mkdir build && cd build
   cmake ..  # Or meson setup build
   make
   ```

3. Run the application (ensure the compositor is Wayland-compatible):
   ```bash
   ./anvilock
   ```

---

## 4. Future

Stay tuned for more updates as we progress through development.

---

