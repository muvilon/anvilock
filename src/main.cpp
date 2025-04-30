#include "anvilock/include/Types.hpp"
#include <anvilock/include/pam/pam.hpp>
#include <anvilock/include/wayland/RegistryHandler.hpp>
#include <anvilock/include/wayland/xdg/SurfaceHandler.hpp>

using namespace anvlk;

using logL = logger::LogLevel;

// This is a basic PAM setup (very naive and impractical) that showcases the
// current progress of the porting process of Anvilock to C++.
//
// Before PAM comes into picture, the Wayland Registry Handler has been implemented completely.
//
// In the future, surface rendering and EGL will be integrated along with the session lock obj.

static auto initWayland(ClientState& state) -> int
{
  logger::log(logL::Debug, state.logCtx, "Initializing Wayland...");
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

static auto initXKB(ClientState& state) -> int
{
  state.xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (!state.xkbContext)
  {
    logger::log(logL::Error, state.logCtx, "Failed to initialize XKB context!");
    return ANVLK_FAILED;
  }

  state.xkbKeymap =
    xkb_keymap_new_from_names(state.xkbContext, nullptr, XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!state.xkbKeymap)
  {
    logger::log(logL::Error, state.logCtx, "Failed to create XKB keymap!");
    return ANVLK_FAILED;
  }

  state.xkbState = xkb_state_new(state.xkbKeymap);
  if (!state.xkbState)
  {
    logger::log(logL::Error, state.logCtx, "Failed to create XKB state!");
    return ANVLK_FAILED;
  }

  return ANVLK_SUCCESS;
}

auto main() -> int
{
  ClientState cs;

  cs.setLogContext(true, "log.txt", true, logger::LogLevel::Debug);

  if (initWayland(cs) == ANVLK_SUCCESS)
  {
    logger::log(logL::Info, cs.logCtx, "{}",
                term::ansi::bold("Successfully initialized Wayland!!"));
  }
  else
  {
    logger::log(logL::Critical, cs.logCtx, "Failed to initialize Wayland!");
  }
  if (initXKB(cs) == ANVLK_SUCCESS)
  {
    logger::log(logL::Info, cs.logCtx, "{}", term::ansi::bold("Successfully initialized XKB!"));
  }
  else
  {
    logger::log(logL::Critical, cs.logCtx, "Failed to initialize XKB!");
  }

  pam::PamAuthenticator auth(cs);
  logger::log(logL::Info, cs.logCtx, "Hello, gib password: ");

  AuthString pwd;
  std::cin >> pwd;

  cs.pam.password = pwd;
  cs.pam.username = "s1dd";

  if (auth.AuthenticateUser())
  {
    logger::log(logL::Info, cs.logCtx, "ok");
  }
  else
  {
    logger::log(logL::Critical, cs.logCtx, "Error while auth");
  }

  return ANVLK_SUCCESS;
}
