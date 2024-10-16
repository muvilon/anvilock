#define _POSIX_C_SOURCE 200809L
#include "client_state.h"
#include "log.h"
#include "pam.h"
#include "protocols/src/xdg-shell-client-protocol.c"
#include "protocols/xdg-shell-client-protocol.h"
#include "shared_mem_handle.h"
#include "wl_buffer_handle.h"
#include "wl_keyboard_handle.h"
#include "wl_pointer_handle.h"
#include "wl_registry_handle.h"
#include "wl_seat_handle.h"
#include "xdg_surface_handle.h"
#include "xdg_wm_base_handle.h"
#include "wl_output_handle.h"
#include <unistd.h>
#include <xkbcommon/xkbcommon.h>
#include "session_lock_handle.h"

/**********************************************
 * @WAYLAND CLIENT EXAMPLE CODE
 **********************************************
 *
 * This program implements a simple Wayland client using the Wayland
 * protocol and the XDG Shell extension. It connects to a Wayland server,
 * creates a window with a checkerboard pattern, and handles pointer and
 * keyboard events.
 *
 * @STRUCTURES AND LISTENERS:
 *
 * 1. **client_state**: A structure that holds the state of the client,
 * including:
 *    - Wayland display, registry, compositor, and other protocol objects.
 *    - XDG surfaces and top-level windows.
 *    - Pointer and keyboard objects.
 *    - Current pointer event information.
 *
 * 2. **pointer_event**: A structure that encapsulates information about
 * pointer events, such as motion, button presses, and axis scrolling.
 *
 * 3. **Listeners**: Functions that respond to various Wayland events.
 *    - **wl_registry_listener**: Listens for global objects added or
 *      removed (e.g., compositor, SHM).
 *    - **wl_seat_listener**: Listens for capabilities of input devices
 *      (e.g., pointer, keyboard).
 *    - **wl_pointer_listener**: Listens for pointer events (enter,
 *      leave, motion, button actions, etc.).
 *    - **wl_keyboard_listener**: Listens for keyboard events (key presses,
 *      keymap updates, etc.).
 *    - **xdg_surface_listener**: Listens for surface configuration events.
 *    - **xdg_wm_base_listener**: Listens for ping requests from the
 *      compositor.
 *
 * @FLOW OF THE PROGRAM:
 *
 * 1. **Initialization**:
 *    - The program starts by connecting to the Wayland display server and
 *      obtaining the registry.
 *    - The registry listener is added to receive global objects.
 *    - A round trip to the display server is performed to get the global
 *      objects.
 *
 * 2. **Creating the Surface**:
 *    - A Wayland surface is created through the compositor.
 *    - An XDG surface is obtained from the XDG shell base, which allows
 *      for proper window management.
 *    - The surface is configured with a title and is committed to the
 *      compositor.
 *
 * 3. **Event Loop**:
 *    - The program enters a loop where it dispatches Wayland events.
 *      This allows the client to respond to pointer and keyboard events as
 *      they occur.
 *
 * 4. **Pointer Events**:
 *    - The pointer listener captures events related to pointer motion,
 *      button presses, and axis scrolling.
 *    - Each event updates the `pointer_event` structure in the client state,
 *      which is then printed to the stderr for debugging.
 *
 * 5. **Keyboard Events**:
 *    - The keyboard listener captures key presses and releases.
 *    - It updates the keyboard state and prints key information to
 *      the stderr.
 *
 * 6. **Buffer Management**:
 *    - A shared memory buffer is created to store pixel data for drawing.
 *    - The `draw_frame` function is called to render a checkerboard pattern
 *      onto this buffer.
 *    - The buffer is attached to the surface and committed to be displayed
 *      on the screen.
 *
 * @CONCLUSION:
 *
 * This program serves as a basic example of how to create a Wayland client,
 * handle input events, and render content using shared memory. It demonstrates
 * the structure of a Wayland client application and how to interact with
 * the Wayland compositor through various protocols and listeners.
 **********************************************/

int main(int argc, char* argv[]) {
    struct client_state state = {0};

    // Initialize and connect to the Wayland display
    state.wl_display = wl_display_connect(NULL);
    state.username   = getlogin();
    log_message(LOG_LEVEL_INFO, "Found User @ %s", state.username);

    if (!state.wl_display) {
        log_message(LOG_LEVEL_ERROR, "Failed to connect to Wayland display\n");
        return -1;
    }

    // Get the registry and set up listeners
    state.wl_registry = wl_display_get_registry(state.wl_display);
    state.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS); 

    // Add listeners for registry objects
    wl_registry_add_listener(state.wl_registry, &wl_registry_listener, &state);

    // Roundtrip to process the registry and get the compositor, shm, seat, etc.
    wl_display_roundtrip(state.wl_display);

    // Create the Wayland surface and initialize XDG shell surface
    state.wl_surface  = wl_compositor_create_surface(state.wl_compositor);
    state.xdg_surface = xdg_wm_base_get_xdg_surface(state.xdg_wm_base, state.wl_surface);
    
    // Add listeners for XDG surface events
    xdg_surface_add_listener(state.xdg_surface, &xdg_surface_listener, &state);

    // Create the XDG toplevel (window management)
    state.xdg_toplevel = xdg_surface_get_toplevel(state.xdg_surface);
    xdg_toplevel_set_title(state.xdg_toplevel, "Screen Lock");

    // Initialize XKB for handling keyboard input
    state.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    state.xkb_keymap = xkb_keymap_new_from_names(state.xkb_context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
    state.xkb_state = xkb_state_new(state.xkb_keymap);

    // Initialize pointer and keyboard state
    state.authenticated = false; // Initialize authentication state

    // Add listeners for seat (input devices like keyboard)
    wl_seat_add_listener(state.wl_seat, &wl_seat_listener, &state);

    // Commit the surface to make it visible
    wl_surface_commit(state.wl_surface);

    // Enter event loop for handling lock state and input
    while (!state.authenticated && wl_display_dispatch(state.wl_display) != -1) {
        // Check if the session needs to be locked
        if (!state.surface_created) {
            initiate_session_lock(&state);
        }
    }

    // Disconnect from the Wayland display
    unlock_and_destroy_session_lock(&state);
    wl_display_roundtrip(state.wl_display);
    wl_display_disconnect(state.wl_display);
    log_message(LOG_LEVEL_SUCCESS, "Unlocking...");
    return 0;
}
