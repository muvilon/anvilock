#include <anvilock/include/LogMacros.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/wayland/seats/KeyboardHandler.hpp>

namespace anvlk::wl
{

void handleBackspace(ClientState& cs, bool ctrl)
{
  if (ctrl)
  {
    // If CTRL is held, clear the password or reset the index
    cs.pamState.clearPassword();
  }
  else if (cs.pamState.passwordIndex > 0)
  {
    // Regular backspace logic: move cursor back and delete last character
    cs.pamState.passwordIndex--;
    cs.pamState.password.resize(cs.pamState.passwordIndex);
  }
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
                     types::wayland::WLSurface_*, types::wayland::WLArray_* /*keys*/)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_KB);
  LOG::DEBUG(cs.logCtx, "Keyboard entered surface!");
  cs.keyboardState.resetState();
  logger::resetCtx(cs.logCtx);
}

void onKeyboardLeave(types::VPtr data, types::wayland::WLKeyboard_*, u32,
                     types::wayland::WLSurface_*)
{
  // No-op for now
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_KB);
  LOG::INFO(cs.logCtx, "Keyboard left surface.");
  cs.keyboardState.resetState();
  logger::resetCtx(cs.logCtx);
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

void onKeyboardKey(types::VPtr data, types::wayland::WLKeyboard_*, u32, u32, u32 key, u32 kbState)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_KB);
  const u32                    keycode = key + XKB_KEYCODE_OFFSET;
  const types::xkb::XKBKeySym_ sym     = xkb_state_key_get_one_sym(cs.xkbState, keycode);
  types::CharArray<8>          utf8Sym{};
  xkb_state_key_get_utf8(cs.xkbState, keycode, utf8Sym.data(), utf8Sym.size());

  bool shouldRender = false; // Flag to track if we need to render

  if (kbState == WL_KEYBOARD_KEY_STATE_PRESSED)
  {
    if (sym == XKB_KEY_Control_L || sym == XKB_KEY_Control_R)
    {
      cs.keyboardState.ctrlHeld = true;
    }
    else if (sym == XKB_KEY_BackSpace)
    {
      handleBackspace(cs, cs.keyboardState.ctrlHeld);
      LOG::DEBUG(cs.logCtx, "Held backspace: ");
      cs.keyboardState.backspaceHeld     = true;
      cs.keyboardState.lastBackspaceTime = SteadyClock::now();
      shouldRender                       = true; // Backspace should trigger a render
    }
    else if (ALLOWED_KEYS.count(sym) == KeycodeStatus::FOUND && cs.pamState.canSeekIndex())
    {
      if (!utf8Sym[0] || utf8Sym[0] == '\0')
        return; // Skip if no character was produced

      auto len = std::strlen(utf8Sym.data());
      if (cs.pamState.canSeekToOffset(len))
      {
        cs.pamState.password.append(utf8Sym.data(), len);
        cs.pamState.seekToIndex(len);
        shouldRender = true; // Adding to password should trigger a render
      }
    }
  }
  else if (kbState == WL_KEYBOARD_KEY_STATE_RELEASED)
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
      cs.keyboardState.resetState();
      if (cs.pamState.passwordIndex > 0)
      {
        cs.initPamAuth();

        if (cs.pamAuth->AuthenticateUser())
        {
          LOG::INFO(cs.logCtx, "Authentication successful.");
          cs.pamState.clearPassword();
          cs.pamState.authState.authSuccess = true;
        }
        else
        {
          LOG::ERROR(cs.logCtx, "Authentication failed. Entered: {}", cs.pamState.password);
          // sanity clear, password is std::moved when AuthenticateUser() is called
          cs.pamState.clearPassword();
          cs.pamState.authState.authFailed = true;
          shouldRender                     = true; // Failed auth should trigger a render
        }

        cs.pamState.firstEnterPress = false;
      }
      shouldRender = true; // Enter key should trigger a render
    }
  }

  // Only render when necessary
  if (shouldRender)
  {
    render::renderLockScreen(cs);
  }

  logger::resetCtx(cs.logCtx);
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
