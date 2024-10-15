#ifndef SESSION_LOCK_HANDLE_H
#define SESSION_LOCK_HANDLE_H

#include "log.h"
#include <string.h>
#include <wayland-client.h>

// Protocol-specific headers (you should replace this with the actual protocol header)
#include "protocols/ext-session-lock-client-protocol.h" // Include the protocol definitions here
#include "protocols/src/ext-session-lock-client-protocol.c"

struct session_lock_state
{
  struct wl_display*      display;      // Wayland display connection
  struct wl_registry*     registry;     // Registry for global objects
  struct wl_seat*         wl_seat;      // Input device seat
  struct wl_session_lock* session_lock; // Session lock interface
};

// Listener for session lock events
static void session_lock_locked(void* data, struct wl_session_lock* lock)
{
  log_message(LOG_LEVEL_INFO, "Session locked");
}

static void session_lock_unlocked(void* data, struct wl_session_lock* lock)
{
  log_message(LOG_LEVEL_INFO, "Session unlocked");
}

// Listener structure for session lock events
static const struct wl_session_lock_listener session_lock_listener = {
  .locked   = session_lock_locked,
  .unlocked = session_lock_unlocked,
};

// Function to bind to the session lock protocol
void bind_session_lock(struct session_lock_state* state)
{
  state->session_lock =
    wl_registry_bind(state->registry,
                     /* name */ 1, // This should be the actual name of the session lock
                     &wl_session_lock_interface, // Use the actual interface here
                     1                           // version
    );

  wl_session_lock_add_listener(state->session_lock, &session_lock_listener, state);
}

#endif // SESSION_LOCK_HANDLE_H
