#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/wayland/seats/KeyboardHandler.hpp>
#include <anvilock/include/wayland/seats/PointerHandler.hpp>

namespace anvlk::wl
{

void onSeatCapabilities(types::VPtr data, types::wayland::WLSeat_* seat, uint32_t capabilities);
void onSeatName(types::VPtr data, types::wayland::WLSeat_* seat, const char* name);

// Listener declaration (global constexpr)
inline constexpr wl_seat_listener kSeatListener{
  .capabilities = onSeatCapabilities,
  .name         = onSeatName,
};

} // namespace anvlk::wl
