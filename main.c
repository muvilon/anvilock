#define _POSIX_C_SOURCE 200809L
#include "client_state.h"
#include "freetype.h"
#include "log.h"
#include "pam.h"
#include "protocols/src/xdg-shell-client-protocol.c"
#include "protocols/xdg-shell-client-protocol.h"
#include "session_lock_handle.h"
#include "shared_mem_handle.h"
#include "wl_buffer_handle.h"
#include "wl_keyboard_handle.h"
#include "wl_output_handle.h"
#include "wl_pointer_handle.h"
#include "wl_registry_handle.h"
#include "wl_seat_handle.h"
#include "xdg_surface_handle.h"
#include "xdg_wm_base_handle.h"
#include <unistd.h>

/**********************************************
 * @HOW ANVILOCK WORKS
 **********************************************
 *
 * This program implements a simple screen lock feature using the Wayland
 * protocol and the XDG Shell extension. It connects to a Wayland server,
 * creates a window for the lock screen, and handles keyboard input for
 * user authentication.
 *
 * @STRUCTURES AND LISTENERS:
 *
 * 1. **client_state**: A structure that holds the state of the client,
 *    including:
 *    - Wayland display, registry, compositor, and other protocol objects.
 *    - XDG surfaces for the lock screen and top-level windows.
 *    - Pointer and keyboard objects.
 *    - Buffer for rendering the lock screen.
 *    - Current user input data (username and password) and authentication status.
 *
 * 2. **Listeners**: Functions that respond to various Wayland events:
 *    - **wl_registry_listener**: Listens for global objects added or
 *      removed (e.g., compositor, SHM).
 *    - **wl_seat_listener**: Listens for capabilities of input devices
 *      (e.g., pointer, keyboard).
 *    - **wl_keyboard_listener**: Listens for keyboard events (key presses,
 *      keymap updates, etc.) and handles user input for unlocking.
 *
 * @FLOW OF THE PROGRAM:
 *
 * 1. **Initialization**:
 *    - The program starts by connecting to the Wayland display server and
 *      obtaining the registry.
 *    - The registry listener is added to receive global objects, which are
 *      necessary for rendering the lock screen.
 *    - A round trip to the display server is performed to get the global
 *      objects needed for window management and input handling.
 *
 * 2. **Creating the Lock Screen Surface**:
 *    - A Wayland surface is created through the compositor for the lock screen.
 *    - An XDG surface is obtained from the XDG shell base, allowing for proper
 *      management of the lock screen window.
 *    - The lock screen is configured with a title and is committed to the
 *      compositor for display.
 *
 * 3. **Event Loop**:
 *    - The program enters an event loop where it dispatches Wayland events.
 *      This enables the client to respond to keyboard input and manage the
 *      lock screen display.
 *
 * 4. **Keyboard Input Handling**:
 *    - The keyboard listener captures key presses and releases.
 *    - When the user types their password, the input is collected into a
 *      buffer (`client_state->password`), with backspace handling to allow
 *      for corrections.
 *    - Special key handling for the Return key triggers authentication logic:
 *      - If the password is non-empty, the program attempts to authenticate
 *        the user against a PAM (Pluggable Authentication Module) service.
 *      - On successful authentication, the program logs the event and
 *        updates the client's state to indicate that the user is authenticated.
 *      - On failure, the password buffer is cleared, and a failure message
 *        is displayed on the lock screen.
 *
 * 5. **Buffer Management**:
 *    - A shared memory buffer is created to store pixel data for drawing the
 *      lock screen.
 *    - The `draw_lock_screen` function is called to render the lock screen
 *      content, including messages for authentication success or failure.
 *    - The buffer is attached to the surface and committed to be displayed
 *      on the screen, ensuring the lock screen reflects the current state
 *      of user input.
 *
 * 6. **User Interaction and Feedback**:
 *    - The lock screen provides visual feedback based on user interactions,
 *      updating in real-time as the user types or if authentication attempts
 *      are made.
 *    - The program manages input states to handle situations where the
 *      backspace key is held down, allowing for continuous deletion until
 *      released.
 *
 * @CONCLUSION:
 *
 * This program serves as a basic example of how to create a Wayland client
 * for a screen lock feature. It handles user input, manages authentication
 * through PAM, and provides visual feedback using shared memory. This example
 * demonstrates the structure and interaction of a screen lock application
 * within the Wayland ecosystem, showcasing the use of various protocols and
 * listeners for input handling and rendering.
 **********************************************/

static int initialize_wayland(struct client_state* state)
{
  state->wl_display = wl_display_connect(NULL);
  if (!state->wl_display)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to connect to Wayland display\n");
    return -1;
  }

  state->wl_registry = wl_display_get_registry(state->wl_display);
  wl_registry_add_listener(state->wl_registry, &wl_registry_listener, state);
  wl_display_roundtrip(state->wl_display); // Get Wayland objects

  state->wl_surface  = wl_compositor_create_surface(state->wl_compositor);
  state->xdg_surface = xdg_wm_base_get_xdg_surface(state->xdg_wm_base, state->wl_surface);
  xdg_surface_add_listener(state->xdg_surface, &xdg_surface_listener, state);

  state->xdg_toplevel = xdg_surface_get_toplevel(state->xdg_surface);
  xdg_toplevel_set_title(state->xdg_toplevel, "Anvilock");

  return 0;
}

static int initialize_xkb(struct client_state* state)
{
  state->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (!state->xkb_context)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to initialize XKB context\n");
    return -1;
  }

  state->xkb_keymap =
    xkb_keymap_new_from_names(state->xkb_context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!state->xkb_keymap)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create XKB keymap\n");
    return -1;
  }

  state->xkb_state = xkb_state_new(state->xkb_keymap);
  if (!state->xkb_state)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create XKB state\n");
    return -1;
  }

  return 0;
}

static int initialize_freetype(struct client_state* state)
{
  int ft = init_freetype();
  if (ft != 0)
  {
    return 0;
  }
  log_message(LOG_LEVEL_ERROR, "Initializing FreeType2 was unsuccessful.\n");
  return -1;
}

static void cleanup(struct client_state* state)
{
  unlock_and_destroy_session_lock(state);
  eglDestroySurface(state->egl_display, state->egl_surface);
  eglDestroyContext(state->egl_display, state->egl_context);
  eglTerminate(state->egl_display);
  wl_display_roundtrip(state->wl_display);
  wl_display_disconnect(state->wl_display);

  FT_Done_Face(ft_face);
  FT_Done_FreeType(ft_library);
}

int main(int argc, char* argv[])
{
  struct client_state state = {0};

  // Initialize logging
  state.pam.username = getlogin();
  log_message(LOG_LEVEL_INFO, "Found User @ %s", state.pam.username);

  // Initialize Wayland
  if (initialize_wayland(&state) != 0)
  {
    cleanup(&state);
    return 1;
  }

  // Initialize XKB for keyboard input
  if (initialize_xkb(&state) != 0)
  {
    cleanup(&state);
    return 1;
  }

  // Initialize FreeType for font rendering
  if (initialize_freetype(&state) != 0)
  {
    cleanup(&state);
    return 1;
  }

  // Commit the surface to make it visible
  wl_surface_commit(state.wl_surface);

  // Event loop to handle input and manage session state
  while (!state.pam.authenticated && wl_display_dispatch(state.wl_display) != -1)
  {
    if (!state.session_lock.surface_created)
    {
      initiate_session_lock(&state);
    }
  }

  // Cleanup after exiting the event loop
  cleanup(&state);
  log_message(LOG_LEVEL_SUCCESS, "Unlocking...");

  return 0;
}
