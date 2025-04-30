#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/wayland/OutputHandler.hpp>
#include <anvilock/include/wayland/seats/SeatHandler.hpp>
#include <anvilock/include/wayland/xdg/WmBaseHandler.hpp>
#include <anvilock/protocols/ext-session-lock-client-protocol.h>

#include <string_view>
#include <wayland-client.h>

namespace anvlk::wl
{

using namespace anvlk;
using logL = logger::LogLevel;

// Registry handler
inline void handleRegistryGlobal(ClientState& cs, wl_registry* registry, uint32_t name,
                                 const char* interface, uint32_t version)
{
  const std::string_view iface{interface};

  logger::log(logL::Info, cs.logCtx, "Binding global: {} (version {})", iface, version);

  if (iface == wl_shm_interface.name)
  {
    cs.wlShm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, 1));
    logger::log(logL::Info, cs.logCtx, "SHM interface bound.");
  }
  else if (iface == wl_compositor_interface.name)
  {
    cs.wlCompositor =
      static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, 4));
    logger::log(logL::Info, cs.logCtx, "Compositor interface bound.");
  }
  else if (iface == xdg_wm_base_interface.name)
  {
    cs.xdgWmBase =
      static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
    xdg_wm_base_add_listener(cs.xdgWmBase, &wl::kWmBaseListener, &cs);
    logger::log(logL::Info, cs.logCtx, "XDG WM Base interface bound.");
  }
  else if (iface == wl_seat_interface.name)
  {
    cs.wlSeat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, 7));
    wl_seat_add_listener(cs.wlSeat, &wl::kSeatListener, &cs);
    logger::log(logL::Info, cs.logCtx, "Seat interface bound.");
  }
  else if (iface == ext_session_lock_manager_v1_interface.name)
  {
    cs.sessionLock.lockManager = static_cast<ext_session_lock_manager_v1*>(
      wl_registry_bind(registry, name, &ext_session_lock_manager_v1_interface, 1));
    logger::log(logL::Info, cs.logCtx, "ext_session_lock_manager interface bound.");
  }
  else if (iface == wl_output_interface.name)
  {
    cs.outputState.wlOutput =
      static_cast<wl_output*>(wl_registry_bind(registry, name, &wl_output_interface, version));
    wl_output_add_listener(cs.outputState.wlOutput, &wl::kOutputListener, &cs);
    logger::log(logL::Info, cs.logCtx, "Output interface bound.");
  }
  else
  {
    logger::log(logL::Warn, cs.logCtx, "Unknown interface: {}", iface);
  }
}

inline void handleRegistryRemove(ClientState& cs, uint32_t name)
{
  logger::log(logL::Info, cs.logCtx, "Removing global: {}", name);
}

inline constexpr static wl_registry_listener kRegistryListener = {
  // Capture reference to cs
  .global = [](void* data, wl_registry* registry, u32 name, const char* interface, u32 version)
  { handleRegistryGlobal(*static_cast<ClientState*>(data), registry, name, interface, version); },
  .global_remove = [](void* data, wl_registry*, u32 name)
  { handleRegistryRemove(*static_cast<ClientState*>(data), name); }};

} // namespace anvlk::wl
