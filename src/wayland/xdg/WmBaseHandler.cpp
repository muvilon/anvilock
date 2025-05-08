#include <anvilock/include/LogMacros.hpp>
#include <anvilock/include/wayland/xdg/WmBaseHandler.hpp>

namespace anvlk::wl
{

static void onXdgWmBasePing(types::VPtr data, anvlk::types::wayland::xdg::XDGWmBase_* xdgWmBase,
                            u32 serial)
{
  auto& cs = *static_cast<ClientState*>(data);
  LOG::INFO(cs.logCtx, "Received ping from xdg_wm_base, sending pong.");
  xdg_wm_base_pong(xdgWmBase, serial);
}

// Listener instance
const xdg_wm_base_listener kWmBaseListener{
  .ping = onXdgWmBasePing,
};

} // namespace anvlk::wl
