#ifndef ANVLK_WAYLAND_SEATS_SEAT_HANDLER_HPP
#define ANVLK_WAYLAND_SEATS_SEAT_HANDLER_HPP

#include <anvilock/ClientState.hpp>
#include <anvilock/Log.hpp>
#include <anvilock/Types.hpp>
#include <anvilock/wayland/seats/KeyboardHandler.hpp>
#include <anvilock/wayland/seats/PointerHandler.hpp>

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

#endif
