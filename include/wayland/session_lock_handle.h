#ifndef SESSION_LOCK_HANDLE_H
#define SESSION_LOCK_HANDLE_H

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "../client_state.h"
#include "../graphics/egl.h"
#include "../log.h"
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
  state->pam.locked          = true;
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
  state->session_lock.surface_dirty = true;

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

  if (!state->output_state.wl_output)
  {
    log_message(LOG_LEVEL_ERROR, "No output available for lock surface");
    return;
  }

  // Create the ext-session-lock surface
  state->session_lock.ext_session_lock_surface = ext_session_lock_v1_get_lock_surface(
    state->session_lock.ext_session_lock, state->wl_surface, state->output_state.wl_output);
  assert(state->session_lock.ext_session_lock_surface);

  ext_session_lock_surface_v1_add_listener(state->session_lock.ext_session_lock_surface,
                                           &ext_session_lock_surface_v1_listener, state);

  init_egl(state);

  // Add Wayland listeners for input devices
  wl_keyboard_add_listener(state->wl_keyboard, &wl_keyboard_listener, state);
  wl_pointer_add_listener(state->wl_pointer, &wl_pointer_listener, state);

  // Mark the surface as created
  state->session_lock.surface_created = true;
}

// Function to initiate the session lock process
static void initiate_session_lock(struct client_state* state)
{
  // Lock the session
  state->session_lock.ext_session_lock =
    ext_session_lock_manager_v1_lock(state->session_lock.ext_session_lock_manager);
  assert(state->session_lock.ext_session_lock);

  // Create the lock surface and trigger lock screen rendering
  create_lock_surface(state);
  render_lock_screen(state);
}

// Function to unlock and destroy the session lock
static void unlock_and_destroy_session_lock(struct client_state* state)
{
  if (state->session_lock.ext_session_lock)
  {
    ext_session_lock_v1_unlock_and_destroy(state->session_lock.ext_session_lock);
    state->session_lock.ext_session_lock = NULL;
    log_message(LOG_LEVEL_INFO, "Session unlocked and lock object destroyed.");
  }
}

#endif // SESSION_LOCK_HANDLE_H
