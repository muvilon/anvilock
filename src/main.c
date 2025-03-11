#include "main.h"

/**********************************************
 * @HOW ANVILOCK WORKS
 **********************************************
 *
 * $ THIS IS OUTDATED WILL UPDATE SOON
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

int main(int argc, char* argv[])
{
  struct client_state state = {0};

  // Initialize logging
  state.pam.username = getlogin();
  log_message(LOG_LEVEL_TRACE, "Session found for user @ [%s]", state.pam.username);

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

  if (initialize_configs(&state) != 0)
  {
    cleanup(&state);
    return 1;
  }

  init_debug(&state);

  initialize_shaders();

  // Commit the surface to make it visible
  wl_surface_commit(state.wl_surface);

  // Event loop to handle input and manage session state
  state.pam.auth_state.auth_success = false;
  while (!state.pam.auth_state.auth_success && wl_display_dispatch(state.wl_display) != -1)
  {
    state.pam.auth_state.auth_failed = false;
    if (!state.session_lock.surface_created)
    {
      initiate_session_lock(&state);
    }
    render_lock_screen(&state);
  }

  // Cleanup after exiting the event loop
  cleanup(&state);
  log_message(LOG_LEVEL_SUCCESS, "Unlocking...");

  return 0;
}
