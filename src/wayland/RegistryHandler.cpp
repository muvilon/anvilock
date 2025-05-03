#include <anvilock/include/wayland/RegistryHandler.hpp>

#include <anvilock/include/Log.hpp>
#include <anvilock/include/wayland/OutputHandler.hpp>
#include <anvilock/include/wayland/seats/SeatHandler.hpp>
#include <anvilock/include/wayland/xdg/WmBaseHandler.hpp>
#include <anvilock/protocols/ext-session-lock-client-protocol.h>

#include <string_view>

namespace anvlk::wl
{

using logL = logger::LogLevel;

void handleRegistryGlobal(ClientState& cs, anvlk::types::wayland::WLRegistry_* registry, u32 name,
                          const char* interface, u32 version)
{
  const std::string_view iface{interface};

  logger::log(logL::Info, cs.logCtx, "Binding global: {} (version {})", iface, version);

  if (iface == wl_shm_interface.name)
  {
    cs.wlShm = static_cast<anvlk::types::wayland::WLShm_*>(
      wl_registry_bind(registry, name, &wl_shm_interface, 1));
    logger::log(logL::Info, cs.logCtx, "SHM interface bound.");
  }
  else if (iface == wl_compositor_interface.name)
  {
    cs.wlCompositor = static_cast<anvlk::types::wayland::WLCompositor_*>(
      wl_registry_bind(registry, name, &wl_compositor_interface, 4));
    logger::log(logL::Info, cs.logCtx, "Compositor interface bound.");
  }
  else if (iface == xdg_wm_base_interface.name)
  {
    cs.xdgWmBase = static_cast<anvlk::types::wayland::xdg::XDGWmBase_*>(
      wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
    xdg_wm_base_add_listener(cs.xdgWmBase, &wl::kWmBaseListener, &cs);
    logger::log(logL::Info, cs.logCtx, "XDG WM Base interface bound.");
  }
  else if (iface == wl_seat_interface.name)
  {
    cs.wlSeat = static_cast<anvlk::types::wayland::WLSeat_*>(
      wl_registry_bind(registry, name, &wl_seat_interface, 7));
    wl_seat_add_listener(cs.wlSeat, &wl::kSeatListener, &cs);
    logger::log(logL::Info, cs.logCtx, "Seat interface bound.");
  }
  else if (iface == ext_session_lock_manager_v1_interface.name)
  {
    cs.sessionLock.lockManager = static_cast<anvlk::types::ext::SessionLockManagerV1_*>(
      wl_registry_bind(registry, name, &ext_session_lock_manager_v1_interface, 1));
    logger::log(logL::Info, cs.logCtx, "ext_session_lock_manager interface bound.");
  }
  else if (iface == wl_output_interface.name)
  {
    cs.outputState.wlOutput = static_cast<anvlk::types::wayland::WLOutput_*>(
      wl_registry_bind(registry, name, &wl_output_interface, version));
    wl_output_add_listener(cs.outputState.wlOutput, &wl::kOutputListener, &cs);
    logger::log(logL::Info, cs.logCtx, "Output interface bound.");
  }
  else
  {
    logger::log(logL::Warn, cs.logCtx, "Unknown interface: {}", iface);
  }
}

void handleRegistryRemove(ClientState& cs, u32 name)
{
  logger::log(logL::Info, cs.logCtx, "Removing global: {}", name);
}

} // namespace anvlk::wl
