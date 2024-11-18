#ifndef WL_KB_HANDLER_H
#define WL_KB_HANDLER_H

#include "client_state.h"
#include "xdg_surface_handle.h"
#include <assert.h>
#include <time.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

static bool            backspace_held = false;
static bool            ctrl_held      = false;
static struct timespec last_backspace_time;

#define BACKSPACE_REPEAT_DELAY_MS 200 // Initial delay before repeating (in ms)
#define BACKSPACE_REPEAT_RATE_MS  50  // Repeat rate while holding (in ms)

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
  /* meh */
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

static long time_diff_ms(struct timespec* start, struct timespec* end)
{
  return (end->tv_sec - start->tv_sec) * 1000 + (end->tv_nsec - start->tv_nsec) / 1000000;
}

static void handle_backspace(struct client_state* client_state, bool ctrl_backspace)
{
  if (ctrl_backspace)
  {
    // Clear the entire password buffer
    client_state->pam.password_index = 0;
  }
  else if (client_state->pam.password_index > 0)
  {
    // Remove one character
    client_state->pam.password_index--;
  }
  client_state->pam.password[client_state->pam.password_index] = '\0';

  // Render the updated lock screen here instead of using draw_lock_screen
  render_lock_screen(client_state);
}

static void handle_backspace_repeat(struct client_state* client_state)
{
  if (backspace_held)
  {
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    long time_since_last_backspace = time_diff_ms(&last_backspace_time, &current_time);

    if (time_since_last_backspace >= BACKSPACE_REPEAT_RATE_MS)
    {
      handle_backspace(client_state, false);
      last_backspace_time = current_time;
    }
  }
}

static void wl_keyboard_key(void* data, struct wl_keyboard* wl_keyboard, uint32_t serial,
                            uint32_t time, uint32_t key, uint32_t state)
{
  struct client_state* client_state = data;
  uint32_t             keycode      = key + 8;
  xkb_keysym_t         sym          = xkb_state_key_get_one_sym(client_state->xkb_state, keycode);

  if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
  {
    if (sym == XKB_KEY_Control_L || sym == XKB_KEY_Control_R)
    {
      ctrl_held = true;
    }
    else if (sym == XKB_KEY_BackSpace)
    {
      handle_backspace(client_state, ctrl_held);
      clock_gettime(CLOCK_MONOTONIC, &last_backspace_time);
      backspace_held = true;
    }
    else if (sym >= XKB_KEY_space && sym <= XKB_KEY_asciitilde &&
             client_state->pam.password_index < sizeof(client_state->pam.password) - 1)
    {
      client_state->pam.password[client_state->pam.password_index++] = (char)sym;
      // Render the updated lock screen here instead of using draw_lock_screen
      render_lock_screen(client_state);
    }
  }
  else if (state == WL_KEYBOARD_KEY_STATE_RELEASED)
  {
    if (sym == XKB_KEY_Control_L || sym == XKB_KEY_Control_R)
    {
      ctrl_held = false;
    }
    else if (sym == XKB_KEY_BackSpace)
    {
      backspace_held = false;
      if (ctrl_held)
      {
        client_state->pam.password_index = 0;
      }
    }
    else if (sym == XKB_KEY_Return)
    {
      if (client_state->pam.password_index > 0)
      {
        client_state->pam.password[client_state->pam.password_index] = '\0';

        if (authenticate_user(client_state->pam.username, client_state->pam.password))
        {
          log_message(LOG_LEVEL_AUTH, "Authentication successful.");
          client_state->pam.auth_state.auth_success = true;
        }
        else
        {
          log_message(LOG_LEVEL_AUTH, "Authentication failed. Try again.");
          client_state->pam.password_index = 0;
          client_state->pam.password[0]    = '\0';

          // Render the updated lock screen here instead of using draw_lock_screen
          client_state->pam.auth_state.auth_failed = true;
          render_lock_screen(client_state);
          client_state->pam.auth_state.auth_failed = false;
        }

        client_state->pam.first_enter_press = false;
      }
    }
  }

  if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
    render_lock_screen(client_state);
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
