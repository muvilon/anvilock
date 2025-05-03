#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>

#include <cassert>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include <chrono>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

namespace anvlk::wl
{

using logL      = logger::LogLevel;
using Clock     = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

namespace
{
constexpr int BACKSPACE_REPEAT_DELAY_MS = 200;
constexpr int BACKSPACE_REPEAT_RATE_MS  = 50;

struct KeyboardState
{
  bool      ctrlHeld          = false;
  bool      backspaceHeld     = false;
  TimePoint lastBackspaceTime = Clock::now();
};

inline auto keyboardState() -> KeyboardState&
{
  static KeyboardState state;
  return state;
}

inline void handleBackspace(ClientState& cs, bool ctrl)
{
  if (ctrl)
  {
    cs.pam.passwordIndex = 0;
  }
  else if (cs.pam.passwordIndex > 0)
  {
    cs.pam.passwordIndex--;
  }
  cs.pam.password[cs.pam.passwordIndex] = '\0';
  //render_lock_screen(&cs);
}

inline void handleBackspaceRepeat(ClientState& cs)
{
  auto& kb = keyboardState();
  if (!kb.backspaceHeld)
    return;

  auto now = Clock::now();
  auto ms =
    std::chrono::duration_cast<std::chrono::milliseconds>(now - kb.lastBackspaceTime).count();
  if (ms >= BACKSPACE_REPEAT_RATE_MS)
  {
    handleBackspace(cs, false);
    kb.lastBackspaceTime = now;
  }
}
} // namespace

inline void onKeyboardEnter(types::VPtr data, types::wayland::WLKeyboard_*, u32,
                            types::wayland::WLSurface_*, types::wayland::WLArray_* keys)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::log(logL::Debug, cs.logCtx, "Keyboard entered surface; keys pressed:");

  for (auto* key = static_cast<u32*>(keys->data);
       key < static_cast<u32*>(keys->data) + keys->size / sizeof(u32); ++key)
  {
    char nameBuf[128];
    auto sym = xkb_state_key_get_one_sym(cs.xkbState, *key + 8);
    xkb_keysym_get_name(sym, nameBuf, sizeof(nameBuf));
    logger::log(logL::Debug, cs.logCtx, "  sym: {:<12} ({})", nameBuf, sym);

    xkb_state_key_get_utf8(cs.xkbState, *key + 8, nameBuf, sizeof(nameBuf));
    logger::log(logL::Debug, cs.logCtx, "  utf8: '{}'", nameBuf);
  }
}

inline void onKeyboardLeave(types::VPtr, types::wayland::WLKeyboard_*, u32,
                            types::wayland::WLSurface_*)
{
  // No-op for now
}

inline void onKeyboardModifiers(types::VPtr data, types::wayland::WLKeyboard_*, u32,
                                u32 mods_depressed, u32 mods_latched, u32 mods_locked, u32 group)
{
  auto& cs = *static_cast<ClientState*>(data);
  xkb_state_update_mask(cs.xkbState, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

inline void onKeyboardRepeatInfo(types::VPtr, types::wayland::WLKeyboard_*, i32, i32)
{
  // Ignored in this context
}

inline void onKeyboardKey(types::VPtr data, types::wayland::WLKeyboard_*, u32, u32, uint32_t key,
                          u32 state)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& kb = keyboardState();

  const u32              keycode = key + 8;
  types::xkb::XKBKeySym_ sym     = xkb_state_key_get_one_sym(cs.xkbState, keycode);

  if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
  {
    if (sym == XKB_KEY_Control_L || sym == XKB_KEY_Control_R)
    {
      kb.ctrlHeld = true;
    }
    else if (sym == XKB_KEY_BackSpace)
    {
      handleBackspace(cs, kb.ctrlHeld);
      kb.backspaceHeld     = true;
      kb.lastBackspaceTime = Clock::now();
    }
    else if (sym >= XKB_KEY_space && sym <= XKB_KEY_asciitilde &&
             static_cast<size_t>(cs.pam.passwordIndex) < sizeof(cs.pam.password) - 1)
    {
      cs.pam.password[cs.pam.passwordIndex++] = static_cast<char>(sym);
      cs.pam.password[cs.pam.passwordIndex]   = '\0';
      //render_lock_screen(&cs);
    }
  }
  else if (state == WL_KEYBOARD_KEY_STATE_RELEASED)
  {
    if (sym == XKB_KEY_Control_L || sym == XKB_KEY_Control_R)
    {
      kb.ctrlHeld = false;
    }
    else if (sym == XKB_KEY_BackSpace)
    {
      kb.backspaceHeld = false;
    }
    else if (sym == XKB_KEY_Return)
    {
      if (cs.pam.passwordIndex > 0)
      {
        cs.pam.password[cs.pam.passwordIndex] = '\0';

        // if (authenticate_user(cs.pam.username, cs.pam.password))
        // {
        //   logger::log(logL::Auth, cs.logCtx, "Authentication successful.");
        //   cs.pam.authState.authSuccess = true;
        // }
        // else
        // {
        //   logger::log(logL::Auth, cs.logCtx, "Authentication failed.");
        //   cs.pam.passwordIndex        = 0;
        //   cs.pam.password[0]          = '\0';
        //   cs.pam.authState.authFailed = true;
        //   //render_lock_screen(&cs);
        //   cs.pam.authState.authFailed = false;
        // }

        cs.pam.firstEnterPress = false;
      }
    }
  }

  //if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
  //render_lock_screen(&cs);
}

inline void onKeyboardKeymap(types::VPtr          data, types::wayland::WLKeyboard_*,
                             [[maybe_unused]] u32 format, i32 fd, u32 size)
{
  auto& cs = *static_cast<ClientState*>(data);
  assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

  char* map = static_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));
  assert(map != MAP_FAILED);

  auto* keymap = xkb_keymap_new_from_string(cs.xkbContext, map, XKB_KEYMAP_FORMAT_TEXT_V1,
                                            XKB_KEYMAP_COMPILE_NO_FLAGS);
  munmap(map, size);
  close(fd);

  auto* state = xkb_state_new(keymap);
  xkb_keymap_unref(cs.xkbKeymap);
  xkb_state_unref(cs.xkbState);
  cs.xkbKeymap = keymap;
  cs.xkbState  = state;
}

inline constexpr wl_keyboard_listener kKeyboardListener = {
  .keymap      = onKeyboardKeymap,
  .enter       = onKeyboardEnter,
  .leave       = onKeyboardLeave,
  .key         = onKeyboardKey,
  .modifiers   = onKeyboardModifiers,
  .repeat_info = onKeyboardRepeatInfo,
};

} // namespace anvlk::wl
