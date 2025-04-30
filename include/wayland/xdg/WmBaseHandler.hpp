#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/protocols/xdg-shell-client-protocol.h>
#include <wayland-client.h>

namespace anvlk::wl
{

using logL = logger::LogLevel;

inline void onXdgWmBasePing(void* data, xdg_wm_base* xdgWmBase, uint32_t serial)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::log(logL::Info, cs.logCtx, "Received ping from xdg_wm_base, sending pong.");
  xdg_wm_base_pong(xdgWmBase, serial);
}

inline constexpr xdg_wm_base_listener kWmBaseListener{
  .ping = onXdgWmBasePing,
};

} // namespace anvlk::wl
