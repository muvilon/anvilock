#ifndef WL_SEAT_HANDLER_H
#define WL_SEAT_HANDLER_H

#include <wayland-client.h>

static void wl_seat_capabilities(void* data, struct wl_seat* wl_seat, uint32_t capabilities)
{
  struct client_state* state = data;
  /* TODO */

  bool have_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;

  if (have_pointer && state->wl_pointer == NULL)
  {
    state->wl_pointer = wl_seat_get_pointer(state->wl_seat);
    wl_pointer_add_listener(state->wl_pointer, &wl_pointer_listener, state);
  }
  else if (!have_pointer && state->wl_pointer != NULL)
  {
    wl_pointer_release(state->wl_pointer);
    state->wl_pointer = NULL;
  }

  bool have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

  if (have_keyboard && state->wl_keyboard == NULL)
  {
    state->wl_keyboard = wl_seat_get_keyboard(state->wl_seat);
    wl_keyboard_add_listener(state->wl_keyboard, &wl_keyboard_listener, state);
  }
  else if (!have_keyboard && state->wl_keyboard != NULL)
  {
    wl_keyboard_release(state->wl_keyboard);
    state->wl_keyboard = NULL;
  }
}

static void wl_seat_name(void* data, struct wl_seat* wl_seat, const char* name)
{
  log_message(LOG_LEVEL_INFO, "seat name: %s", name);
}

static const struct wl_seat_listener wl_seat_listener = {
  .capabilities = wl_seat_capabilities,
  .name         = wl_seat_name,
};

#endif
