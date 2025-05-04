#include <anvilock/include/GlobalFuncs.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/config/ConfigHandler.hpp>
#include <anvilock/include/freetype/FreeTypeHandler.hpp>
#include <anvilock/include/pam/PamAuthenticator.hpp>
#include <anvilock/include/shaders/ShaderHandler.hpp>
#include <anvilock/include/wayland/RegistryHandler.hpp>
#include <anvilock/include/wayland/session-lock/SessionLockHander.hpp>
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

void setHomeDir(ClientState& cs)
{
  types::Directory home = anvlk::utils::getHomeDir();

  if (!home.c_str())
  {
    logger::log(logger::LogLevel::Error, cs.logCtx, "Home directory (env var) not found!");
    return;
  }

  cs.homeDir = home;
  logger::log(logger::LogLevel::Info, cs.logCtx, "Home directory found: '{}'", cs.homeDir);
}

auto main() -> int
{
  ClientState cs;

  setHomeDir(cs);

  cs.setLogContext(true, "log.txt", true, logger::LogLevel::Debug);

  if (initWayland(cs) == ANVLK_SUCCESS)
  {
    logger::log(logL::Info, cs.logCtx, logger::LogStyle::COLOR_BOLD,
                "Successfully initialized Wayland!!");
  }
  else
  {
    logger::log(logL::Error, cs.logCtx, "Failed to initialize Wayland!");
  }
  if (initXKB(cs) == ANVLK_SUCCESS)
  {
    logger::log(logL::Info, cs.logCtx, logger::LogStyle::COLOR_BOLD,
                "Successfully initialized XKB!");
  }
  else
  {
    logger::log(logL::Error, cs.logCtx, "Failed to initialize XKB!");
  }

  // If the class does not take the entire ClientState reference,
  //
  // decl the log context switch in main itself!
  logger::switchCtx(cs.logCtx, anvlk::logger::LogCategory::CONFIG);

  anvlk::cfg::ConfigLoader loader(cs.logCtx, cs.homeDir);
  cs.userConfig = loader.load();

  logger::switchCtx(cs.logCtx, anvlk::logger::LogCategory::FREETYPE);

  anvlk::freetype::FreeTypeHandler freeType(cs.logCtx, cs.userConfig.font.path);

  cs.freeTypeState.ftFace    = freeType.face();
  cs.freeTypeState.ftLibrary = freeType.library();

  logger::switchCtx(cs.logCtx, anvlk::logger::LogCategory::SHADERS);

  anvlk::gfx::ShaderManager shaderMgr(cs.homeDir, cs.logCtx);
  if (!shaderMgr.isValid())
  {
    std::exit(EXIT_FAILURE);
  }

  logger::switchCtx(cs.logCtx, anvlk::logger::LogCategory::PAM);

  anvlk::pam::PamAuthenticator auth(cs);
  logger::log(logL::Info, cs.logCtx, logger::LogStyle::UNDERLINE, "Hello, gib password: ");

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
    logger::log(logL::Error, cs.logCtx, logger::LogStyle::COLOR_BOLD, "Error while auth");
  }

  cs.logCtx.resetContext();

  return ANVLK_SUCCESS;
}
