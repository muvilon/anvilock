#ifndef ANVLK_SRC_HPP
#define ANVLK_SRC_HPP

#include <anvilock/include/GlobalFuncs.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/config/ConfigHandler.hpp>
#include <anvilock/include/freetype/FreeTypeHandler.hpp>
#include <anvilock/include/pam/PamAuthenticator.hpp>
#include <anvilock/include/renderer/EGL.hpp>
#include <anvilock/include/runtime/LogUtils.hpp>
#include <anvilock/include/shaders/ShaderHandler.hpp>
#include <anvilock/include/wayland/RegistryHandler.hpp>
#include <anvilock/include/wayland/session-lock/SessionLockHandler.hpp>
#include <anvilock/include/wayland/xdg/SurfaceHandler.hpp>

using namespace anvlk;

inline auto initWayland(ClientState& state) -> int
{
  LOG::DEBUG(state.logCtx, "Initializing Wayland...");
  state.wlDisplay = wl_display_connect(nullptr);
  if (!state.wlDisplay)
  {
    return ANVLK_UNDEFINED;
  }

  state.wlRegistry = wl_display_get_registry(state.wlDisplay);
  wl_registry_add_listener(state.wlRegistry, &wl::kRegistryListener, &state);
  wl_display_roundtrip(state.wlDisplay); // Get Wayland objects

  state.wlSurface  = wl_compositor_create_surface(state.wlCompositor);
  state.xdgSurface = xdg_wm_base_get_xdg_surface(state.xdgWmBase, state.wlSurface);
  xdg_surface_add_listener(state.xdgSurface, &wl::kXdgSurfaceListener, &state);

  state.xdgToplevel = xdg_surface_get_toplevel(state.xdgSurface);
  xdg_toplevel_set_title(state.xdgToplevel, "Anvilock");

  return ANVLK_SUCCESS;
}

inline auto initXKB(ClientState& state) -> int
{
  state.xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (!state.xkbContext)
  {
    LOG::ERROR(state.logCtx, "Failed to initialize XKB context!");
    return ANVLK_FAILED;
  }

  state.xkbKeymap =
    xkb_keymap_new_from_names(state.xkbContext, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!state.xkbKeymap)
  {
    LOG::ERROR(state.logCtx, "Failed to create XKB keymap!");
    return ANVLK_FAILED;
  }

  state.xkbState = xkb_state_new(state.xkbKeymap);
  if (!state.xkbState)
  {
    LOG::ERROR(state.logCtx, "Failed to create XKB state!");
    return ANVLK_FAILED;
  }

  return ANVLK_SUCCESS;
}

inline void setHomeDir(ClientState& cs)
{
  types::Directory home = anvlk::utils::getHomeDir();
  if (!home.c_str())
  {
    logger::log(logger::LogLevel::Error, cs.logCtx, "Home directory (env var) not found!");
    return;
  }

  cs.setLogContext(true, runtime::logger::generateLogFilePath(home.c_str()), true,
                   runtime::logger::getLogLevelFromEnv());

  cs.homeDir = home;
  logger::log(logger::LogLevel::Info, cs.logCtx, "Home directory found: '{}'", cs.homeDir);
}

#endif
