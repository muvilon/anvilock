#ifndef SESSION_LOCK_HANDLE_H
#define SESSION_LOCK_HANDLE_H

#include "client_state.h"
#include "log.h"
#include "shared_mem_handle.h"
#include "wl_buffer_handle.h"
#include "wl_keyboard_handle.h" // Include the keyboard handler
#include "wl_pointer_handle.h"  // Include the pointer handler
#include "wl_registry_handle.h" // For handling registry
#include "xdg_surface_handle.h" // For rendering the lock screen
#include <string.h>
#include <wayland-client.h>

// Include the ext-session-lock protocol headers

static void ext_session_lock_v1_handle_locked(void* data, struct ext_session_lock_v1* lock)
{
  struct client_state* state = data;
  state->locked              = true;
}

static void ext_session_lock_v1_handle_finished(void* data, struct ext_session_lock_v1* lock)
{
  log_message(LOG_LEVEL_ERROR, "Failed to lock session -- "
                               "is another lockscreen running?");
  exit(2);
}

static const struct ext_session_lock_v1_listener ext_session_lock_v1_listener = {
  .locked   = ext_session_lock_v1_handle_locked,
  .finished = ext_session_lock_v1_handle_finished,
};

// Function for rendering the lock screen
void render_lock_screen(struct client_state* state)
{
  // Ensure surface exists
  if (!state->wl_surface)
  {
    log_message(LOG_LEVEL_ERROR, "No surface to render lock screen");
    return;
  }

  // Create and draw lock screen buffer
  struct wl_buffer* buffer = draw_lock_screen(state, NULL);
  if (!buffer)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create buffer for lock screen");
    return;
  }

  // Attach the buffer to the surface and commit
  wl_surface_attach(state->wl_surface, buffer, 0, 0);
  wl_surface_damage(state->wl_surface, 0, 0, state->output_state.width,
                    state->output_state.height); // Notify the compositor
  wl_surface_commit(state->wl_surface);
}

// Listener callback for ext-session-lock surface configuration
static void
ext_session_lock_surface_v1_handle_configure(void*                               data,
                                             struct ext_session_lock_surface_v1* lock_surface,
                                             uint32_t serial, uint32_t width, uint32_t height)
{

  struct client_state* state = data;
  state->output_state.width  = width;
  state->output_state.height = height;

  // Acknowledge the configuration
  ext_session_lock_surface_v1_ack_configure(lock_surface, serial);

  // Mark surface as dirty for re-rendering
  state->surface_dirty = true;

  // Render the lock screen once the surface is configured
  render_lock_screen(state);
}

// Listener for the session lock surface
static const struct ext_session_lock_surface_v1_listener ext_session_lock_surface_v1_listener = {
  .configure = ext_session_lock_surface_v1_handle_configure,
};

// Function to create the lock surface
static void create_lock_surface(struct client_state* state)
{
  // Create a Wayland surface for the lock screen
  state->wl_surface = wl_compositor_create_surface(state->wl_compositor);
  assert(state->wl_surface);

  // Ensure that output is set correctly in client_state
  if (!state->output_state.wl_output)
  {
    log_message(LOG_LEVEL_ERROR, "No output available for lock surface");
    return;
  }

  // Create the ext-session-lock surface
  state->ext_session_lock_surface_v1 = ext_session_lock_v1_get_lock_surface(
    state->ext_session_lock_v1, state->wl_surface, state->output_state.wl_output);
  assert(state->ext_session_lock_surface_v1);

  // Add listener for configure events
  ext_session_lock_surface_v1_add_listener(state->ext_session_lock_surface_v1,
                                           &ext_session_lock_surface_v1_listener, state);

  // Use the existing keyboard listener
  wl_keyboard_add_listener(state->wl_keyboard, &wl_keyboard_listener, state);

  // Use the existing pointer listener
  wl_pointer_add_listener(state->wl_pointer, &wl_pointer_listener, state);

  // Mark the surface as created
  state->surface_created = true;
}

// Function to initiate the session lock process
static void initiate_session_lock(struct client_state* state)
{
  // Lock the session
  state->ext_session_lock_v1 = ext_session_lock_manager_v1_lock(state->ext_session_lock_manager_v1);
  assert(state->ext_session_lock_v1);

  // Create the lock surface and trigger lock screen rendering
  create_lock_surface(state);
}

// Function to unlock and destroy the session lock
static void unlock_and_destroy_session_lock(struct client_state* state)
{
  if (state->ext_session_lock_v1)
  {
    ext_session_lock_v1_unlock_and_destroy(state->ext_session_lock_v1);
    state->ext_session_lock_v1 = NULL;
    log_message(LOG_LEVEL_INFO, "Session unlocked and lock object destroyed.");
  }
}

#endif // SESSION_LOCK_HANDLE_H
