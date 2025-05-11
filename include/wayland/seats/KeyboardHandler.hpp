#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/renderer/EGL.hpp>

#include <cassert>
#include <unordered_set>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include <sys/mman.h>
#include <unistd.h>

namespace anvlk::wl
{

inline constexpr int CONST_UTF8_SIZE = 128;

// XKB keycodes start at 8 due to legacy X11 protocol design.
inline constexpr u32 XKB_KEYCODE_OFFSET = 8;

inline const std::unordered_set<types::xkb::XKBKeySym_> ALLOWED_KEYS = {
  // Digits
  XKB_KEY_0 /* 0 */, XKB_KEY_1 /* 1 */, XKB_KEY_2 /* 2 */, XKB_KEY_3 /* 3 */, XKB_KEY_4 /* 4 */,
  XKB_KEY_5 /* 5 */, XKB_KEY_6 /* 6 */, XKB_KEY_7 /* 7 */, XKB_KEY_8 /* 8 */, XKB_KEY_9 /* 9 */,

  // Uppercase letters
  XKB_KEY_A /* A */, XKB_KEY_B /* B */, XKB_KEY_C /* C */, XKB_KEY_D /* D */, XKB_KEY_E /* E */,
  XKB_KEY_F /* F */, XKB_KEY_G /* G */, XKB_KEY_H /* H */, XKB_KEY_I /* I */, XKB_KEY_J /* J */,
  XKB_KEY_K /* K */, XKB_KEY_L /* L */, XKB_KEY_M /* M */, XKB_KEY_N /* N */, XKB_KEY_O /* O */,
  XKB_KEY_P /* P */, XKB_KEY_Q /* Q */, XKB_KEY_R /* R */, XKB_KEY_S /* S */, XKB_KEY_T /* T */,
  XKB_KEY_U /* U */, XKB_KEY_V /* V */, XKB_KEY_W /* W */, XKB_KEY_X /* X */, XKB_KEY_Y /* Y */,
  XKB_KEY_Z /* Z */,

  // Lowercase letters
  XKB_KEY_a /* a */, XKB_KEY_b /* b */, XKB_KEY_c /* c */, XKB_KEY_d /* d */, XKB_KEY_e /* e */,
  XKB_KEY_f /* f */, XKB_KEY_g /* g */, XKB_KEY_h /* h */, XKB_KEY_i /* i */, XKB_KEY_j /* j */,
  XKB_KEY_k /* k */, XKB_KEY_l /* l */, XKB_KEY_m /* m */, XKB_KEY_n /* n */, XKB_KEY_o /* o */,
  XKB_KEY_p /* p */, XKB_KEY_q /* q */, XKB_KEY_r /* r */, XKB_KEY_s /* s */, XKB_KEY_t /* t */,
  XKB_KEY_u /* u */, XKB_KEY_v /* v */, XKB_KEY_w /* w */, XKB_KEY_x /* x */, XKB_KEY_y /* y */,
  XKB_KEY_z /* z */,

  // Special characters
  XKB_KEY_exclam /* ! */, XKB_KEY_at /* @ */, XKB_KEY_numbersign /* # */, XKB_KEY_dollar /* $ */,
  XKB_KEY_percent /* % */, XKB_KEY_ampersand /* & */, XKB_KEY_apostrophe /* ' */,
  XKB_KEY_parenleft /* ( */, XKB_KEY_parenright /* ) */, XKB_KEY_asterisk /* * */,
  XKB_KEY_plus /* + */, XKB_KEY_comma /* , */, XKB_KEY_minus /* - */, XKB_KEY_period /* . */,
  XKB_KEY_slash /* / */, XKB_KEY_semicolon /* ; */, XKB_KEY_less /* < */, XKB_KEY_equal /* = */,
  XKB_KEY_greater /* > */, XKB_KEY_question /* ? */, XKB_KEY_bracketleft /* [ */,
  XKB_KEY_backslash /* \ */, XKB_KEY_bracketright /* ] */, XKB_KEY_asciicircum /* ^ */,
  XKB_KEY_underscore /* _ */, XKB_KEY_grave /* ` */, XKB_KEY_braceleft /* { */, XKB_KEY_bar /* | */,
  XKB_KEY_braceright /* } */, XKB_KEY_asciitilde /* ~ */,

  // Misc
  XKB_KEY_space

};

enum KeycodeStatus
{
  FOUND     = 1,
  NOT_FOUND = 0,
  LOST      = -1
};

inline void onKeyboardEnter(types::VPtr data, types::wayland::WLKeyboard_*, u32,
                            types::wayland::WLSurface_*, types::wayland::WLArray_* /*keys*/)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_KB);
  LOG::DEBUG(cs.logCtx, "Keyboard entered surface!");
  cs.keyboardState.resetState();
  logger::resetCtx(cs.logCtx);
}

inline void onKeyboardLeave(types::VPtr data, types::wayland::WLKeyboard_*, u32,
                            types::wayland::WLSurface_*)
{
  // No-op for now
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_KB);
  LOG::INFO(cs.logCtx, "Keyboard left surface.");
  cs.keyboardState.resetState();
  logger::resetCtx(cs.logCtx);
}

inline void onKeyboardModifiers(types::VPtr data, types::wayland::WLKeyboard_*, u32,
                                u32 mods_depressed, u32 mods_latched, u32 mods_locked, u32 group)
{
  auto& cs = *static_cast<ClientState*>(data);
  xkb_state_update_mask(cs.xkbState, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

inline void onKeyboardRepeatInfo(anvlk::types::VPtr, anvlk::types::wayland::WLKeyboard_*, i32, i32)
{
  // Ignored in this context
}

void onKeyboardKey(types::VPtr data, types::wayland::WLKeyboard_*, u32, u32, u32 key, u32 kbState);
void onKeyboardKeymap(types::VPtr data, types::wayland::WLKeyboard_*, [[maybe_unused]] u32 format,
                      i32 fd, u32 size);

inline constexpr wl_keyboard_listener kKeyboardListener = {
  .keymap      = onKeyboardKeymap,
  .enter       = onKeyboardEnter,
  .leave       = onKeyboardLeave,
  .key         = onKeyboardKey,
  .modifiers   = onKeyboardModifiers,
  .repeat_info = onKeyboardRepeatInfo,
};

} // namespace anvlk::wl
