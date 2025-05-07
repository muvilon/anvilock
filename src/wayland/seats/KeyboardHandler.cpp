#include <anvilock/include/Types.hpp>
#include <anvilock/include/wayland/seats/KeyboardHandler.hpp>

namespace anvlk::wl
{

void handleBackspace(ClientState& cs, bool ctrl)
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
  render::renderLockScreen(cs);
}

void handleBackspaceRepeat(ClientState& cs)
{
  if (!cs.keyboardState.backspaceHeld)
    return;

  auto now = SteadyClock::now();
  auto ms =
    std::chrono::duration_cast<std::chrono::milliseconds>(now - cs.keyboardState.lastBackspaceTime)
      .count();
  if (ms >= BACKSPACE_REPEAT_RATE_MS)
  {
    handleBackspace(cs, false);
    cs.keyboardState.lastBackspaceTime = now;
  }
}

void onKeyboardEnter(types::VPtr data, types::wayland::WLKeyboard_*, u32,
                     types::wayland::WLSurface_*, types::wayland::WLArray_* keys)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::log(logL::Debug, cs.logCtx, "Keyboard entered surface; keys pressed:");

  for (auto* key = static_cast<u32*>(keys->data);
       key < static_cast<u32*>(keys->data) + keys->size / sizeof(u32); ++key)
  {
    const u32 xkbKeycode = *key + XKB_KEYCODE_OFFSET;

    types::CharArray<CONST_UTF8_SIZE> nameBuf{};

    const auto sym = xkb_state_key_get_one_sym(cs.xkbState, xkbKeycode);
    xkb_keysym_get_name(sym, nameBuf.data(), nameBuf.size());
    logger::log(logL::Debug, cs.logCtx, "  sym: {:<12} ({})", nameBuf.data(), sym);

    xkb_state_key_get_utf8(cs.xkbState, xkbKeycode, nameBuf.data(), nameBuf.size());
    logger::log(logL::Debug, cs.logCtx, "  utf8: '{}'", nameBuf.data());
  }
}

void onKeyboardLeave(types::VPtr, types::wayland::WLKeyboard_*, u32, types::wayland::WLSurface_*)
{
  // No-op for now
}

void onKeyboardModifiers(types::VPtr data, types::wayland::WLKeyboard_*, u32, u32 mods_depressed,
                         u32 mods_latched, u32 mods_locked, u32 group)
{
  auto& cs = *static_cast<ClientState*>(data);
  xkb_state_update_mask(cs.xkbState, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

void onKeyboardRepeatInfo(anvlk::types::VPtr, anvlk::types::wayland::WLKeyboard_*, i32, i32)
{
  // Ignored in this context
}

void onKeyboardKey(types::VPtr data, types::wayland::WLKeyboard_*, u32, u32, uint32_t key,
                   u32 state)
{
  auto&                  cs      = *static_cast<ClientState*>(data);
  const u32              keycode = key + XKB_KEYCODE_OFFSET;
  types::xkb::XKBKeySym_ sym     = xkb_state_key_get_one_sym(cs.xkbState, keycode);

  if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
  {
    if (sym == XKB_KEY_Control_L || sym == XKB_KEY_Control_R)
    {
      cs.keyboardState.ctrlHeld = true;
    }
    else if (sym == XKB_KEY_BackSpace)
    {
      handleBackspace(cs, cs.keyboardState.ctrlHeld);
      cs.keyboardState.backspaceHeld     = true;
      cs.keyboardState.lastBackspaceTime = SteadyClock::now();
    }
    else if (ALLOWED_KEYS.count(sym) == KeycodeStatus::FOUND && cs.pam.canSeekIndex())
    {
      types::CharArray<8> utf8Sym{};
      xkb_state_key_get_utf8(cs.xkbState, keycode, utf8Sym.data(), utf8Sym.size());

      if (!utf8Sym[0])
        return; // Skip if no character was produced

      auto len = std::strlen(utf8Sym.data());
      if (cs.pam.canSeekToIndex(len))
      {
        cs.pam.password.append(utf8Sym.data(), len);
        cs.pam.seekToIndex(len);
        render::renderLockScreen(cs);
      }
    }
  }
  else if (state == WL_KEYBOARD_KEY_STATE_RELEASED)
  {
    if (sym == XKB_KEY_Control_L || sym == XKB_KEY_Control_R)
    {
      cs.keyboardState.ctrlHeld = false;
    }
    else if (sym == XKB_KEY_BackSpace)
    {
      cs.keyboardState.backspaceHeld = false;
    }
    else if (sym == XKB_KEY_Return)
    {
      if (cs.pam.passwordIndex > 0)
      {
        cs.initPamAuth();

        if (cs.pamAuth->AuthenticateUser())
        {
          logger::log(logL::Info, cs.logCtx, "Authentication successful.");
          cs.pam.authState.authSuccess = true;
        }
        else
        {
          logger::log(logL::Error, cs.logCtx, "Authentication failed.");
          cs.pam.clearPassword();
          cs.pam.authState.authFailed = true;
          render::renderLockScreen(cs);
          cs.pam.authState.authFailed = false;
        }

        cs.pam.firstEnterPress = false;
      }
    }
  }

  if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
    render::renderLockScreen(cs);
}

void onKeyboardKeymap(types::VPtr data, types::wayland::WLKeyboard_*, [[maybe_unused]] u32 format,
                      i32 fd, u32 size)
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

} // namespace anvlk::wl
