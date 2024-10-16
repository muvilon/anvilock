#ifndef WL_KB_HANDLER_H
#define WL_KB_HANDLER_H

#include "client_state.h"
#include "xdg_surface_handle.h"
#include <assert.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

static void wl_keyboard_leave(void* data, struct wl_keyboard* wl_keyboard, uint32_t serial,
                              struct wl_surface* surface)
{
  log_message(LOG_LEVEL_WARN, "keyboard leave");
}

static void wl_keyboard_modifiers(void* data, struct wl_keyboard* wl_keyboard, uint32_t serial,
                                  uint32_t mods_depressed, uint32_t mods_latched,
                                  uint32_t mods_locked, uint32_t group)
{
  struct client_state* client_state = data;
  xkb_state_update_mask(client_state->xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0,
                        group);
}

static void wl_keyboard_repeat_info(void* data, struct wl_keyboard* wl_keyboard, int32_t rate,
                                    int32_t delay)
{
  /* Left as an exercise for the reader */
}

static void wl_keyboard_enter(void* data, struct wl_keyboard* wl_keyboard, uint32_t serial,
                              struct wl_surface* surface, struct wl_array* keys)
{
  struct client_state* client_state = data;
  log_message(LOG_LEVEL_DEBUG, "keyboard enter; keys pressed are:");
  uint32_t* key;
  wl_array_for_each(key, keys)
  {
    char         buf[128];
    xkb_keysym_t sym = xkb_state_key_get_one_sym(client_state->xkb_state, *key + 8);
    xkb_keysym_get_name(sym, buf, sizeof(buf));
    log_message(LOG_LEVEL_DEBUG, "sym: %-12s (%d), ", buf, sym);
    xkb_state_key_get_utf8(client_state->xkb_state, *key + 8, buf, sizeof(buf));
    log_message(LOG_LEVEL_DEBUG, "utf8: '%s'\n", buf);
  }
}

static void wl_keyboard_key(void* data, struct wl_keyboard* wl_keyboard, uint32_t serial,
                            uint32_t time, uint32_t key, uint32_t state)
{
  struct client_state* client_state = data;
  char                 buf[128];
  uint32_t             keycode = key + 8;
  xkb_keysym_t         sym     = xkb_state_key_get_one_sym(client_state->xkb_state, keycode);

  const char* action = (state == WL_KEYBOARD_KEY_STATE_PRESSED) ? "PRESSED" : "RELEASED";

  if (state == WL_KEYBOARD_KEY_STATE_RELEASED)
  {
    if (sym == XKB_KEY_Return)
    {
      if (client_state->password_index > 0)
      {
        client_state->password[client_state->password_index] = '\0'; // Null-terminate the password

        // Attempt PAM authentication
        if (authenticate_user(client_state->username, client_state->password))
        {
          log_message(LOG_LEVEL_AUTH, "Authentication successful.\n");
          client_state->authenticated = true;
        }
        else
        {
          log_message(LOG_LEVEL_AUTH, "Authentication failed. Try again.\n");
          client_state->password_index = 0; // Reset the password input
          client_state->password[client_state->password_index] = '\0';
          
          struct wl_buffer* buffer = draw_lock_screen(client_state, "Authentication failed. Try again.");
          if (!buffer) {
              log_message(LOG_LEVEL_ERROR, "Failed to create buffer for lock screen");
              return;
          }

          wl_surface_attach(client_state->wl_surface, buffer, 0, 0);
          wl_surface_commit(client_state->wl_surface);
          return;
        }

        // Reset the flag to allow new input after an attempt
        client_state->firstEnterPress = false; // Allow for new password entry
      }
    }
    else if (sym == XKB_KEY_BackSpace && client_state->password_index > 0)
    {
      client_state->password[--client_state->password_index] = '\0'; // Handle backspace
    }
    else if (sym >= XKB_KEY_space && sym <= XKB_KEY_asciitilde &&
             client_state->password_index < sizeof(client_state->password) - 1)
    {
      // Capture character input
      client_state->password[client_state->password_index++] = (char)sym;
    }
    struct wl_buffer* buffer = draw_lock_screen(client_state, NULL);
    if (!buffer) {
        log_message(LOG_LEVEL_ERROR, "Failed to create buffer for lock screen");
        return;
    }

    wl_surface_attach(client_state->wl_surface, buffer, 0, 0);
    wl_surface_commit(client_state->wl_surface);
  }
}

static void wl_keyboard_keymap(void* data, struct wl_keyboard* wl_keyboard, uint32_t format,
                               int32_t fd, uint32_t size)
{
  struct client_state* client_state = data;
  assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

  char* map_shm = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
  assert(map_shm != MAP_FAILED);

  struct xkb_keymap* xkb_keymap = xkb_keymap_new_from_string(
    client_state->xkb_context, map_shm, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
  munmap(map_shm, size);
  close(fd);

  struct xkb_state* xkb_state = xkb_state_new(xkb_keymap);
  xkb_keymap_unref(client_state->xkb_keymap);
  xkb_state_unref(client_state->xkb_state);
  client_state->xkb_keymap = xkb_keymap;
  client_state->xkb_state  = xkb_state;
}

static const struct wl_keyboard_listener wl_keyboard_listener = {
  .keymap      = wl_keyboard_keymap,
  .enter       = wl_keyboard_enter,
  .leave       = wl_keyboard_leave,
  .key         = wl_keyboard_key,
  .modifiers   = wl_keyboard_modifiers,
  .repeat_info = wl_keyboard_repeat_info,
};

#endif
