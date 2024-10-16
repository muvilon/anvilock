#ifndef WL_REG_HANDLER_H
#define WL_REG_HANDLER_H

#include "log.h"
#include "wl_seat_handle.h"
#include "wl_output_handle.h"
#include "xdg_wm_base_handle.h"
#include "protocols/ext-session-lock-client-protocol.h"
#include "protocols/src/ext-session-lock-client-protocol.c"
#include <wayland-client.h>

static void registry_global(void* data, struct wl_registry* wl_registry, uint32_t name,
                            const char* interface, uint32_t version)
{
  struct client_state* state = data;

  // Log the binding of global objects
  log_message(LOG_LEVEL_INFO, "Binding global: %s (version %u)", interface, version);

  if (strcmp(interface, wl_shm_interface.name) == 0)
  {
    state->wl_shm = wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
    log_message(LOG_LEVEL_INFO, "SHM interface bound.");
  }
  else if (strcmp(interface, wl_compositor_interface.name) == 0)
  {
    state->wl_compositor = wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4);
    log_message(LOG_LEVEL_INFO, "Compositor interface bound.");
  }
  else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
  {
    state->xdg_wm_base = wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener(state->xdg_wm_base, &xdg_wm_base_listener, state);
    log_message(LOG_LEVEL_INFO, "XDG WM Base interface bound.");
  }
  else if (strcmp(interface, wl_seat_interface.name) == 0)
  {
    state->wl_seat = wl_registry_bind(wl_registry, name, &wl_seat_interface, 7);
    wl_seat_add_listener(state->wl_seat, &wl_seat_listener, state);
    log_message(LOG_LEVEL_INFO, "Seat interface bound.");
  }
  else if (strcmp(interface, ext_session_lock_manager_v1_interface.name) == 0)
  {
    state->ext_session_lock_manager_v1 = wl_registry_bind(wl_registry, name,
				&ext_session_lock_manager_v1_interface, 1);
    log_message(LOG_LEVEL_INFO, "ext_session_lock_manager interface bound.");
  }
  else if (strcmp(interface, wl_output_interface.name) == 0)
  {
    state->output_state.wl_output = wl_registry_bind(wl_registry, name, &wl_output_interface, version);
    wl_output_add_listener(state->output_state.wl_output, &wl_output_listener, state);
    log_message(LOG_LEVEL_INFO, "Output interface bound.");
  }
  else
  {
    log_message(LOG_LEVEL_WARN, "Unknown interface: %s", interface);
  }
}

static void registry_global_remove(void* data, struct wl_registry* wl_registry, uint32_t name)
{
  // Log the removal of global objects
  log_message(LOG_LEVEL_INFO, "Removing global: %u", name);
  // This space deliberately left blank
}

static const struct wl_registry_listener wl_registry_listener = {
  .global        = registry_global,
  .global_remove = registry_global_remove,
};

#endif // WL_REG_HANDLER_H
