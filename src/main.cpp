#include "anvilock/include/runtime/LogLevel.hpp"
#include <anvilock/include/GlobalFuncs.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/config/ConfigHandler.hpp>
#include <anvilock/include/freetype/FreeTypeHandler.hpp>
#include <anvilock/include/pam/PamAuthenticator.hpp>
#include <anvilock/include/renderer/EGL.hpp>
#include <anvilock/include/shaders/ShaderHandler.hpp>
#include <anvilock/include/wayland/RegistryHandler.hpp>
#include <anvilock/include/wayland/session-lock/SessionLockHandler.hpp>
#include <anvilock/include/wayland/xdg/SurfaceHandler.hpp>

using namespace anvlk;

static auto initWayland(ClientState& state) -> int
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

static auto initXKB(ClientState& state) -> int
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
  cs.pamState.authState.authSuccess = false;

  cs.setLogContext(true, "log.txt", true, runtime::logger::getLogLevelFromEnv());
  setHomeDir(cs);
  auto usernameOpt = utils::getCurrentUsername();
  if (!usernameOpt)
  {
    LOG::ERROR(cs.logCtx, "Did not find a valid username! Exiting...");
    std::exit(EXIT_FAILURE);
  }

  cs.pamState.username = *usernameOpt;

  LOG::INFO(cs.logCtx, "Session Lock JOB requested by user @: '{}'", cs.pamState.username);

  if (initWayland(cs) == ANVLK_SUCCESS)
  {
    LOG::INFO(cs.logCtx, logger::LogStyle::COLOR_BOLD, "Successfully initialized Wayland!!");
  }
  else
  {
    LOG::ERROR(cs.logCtx, "Failed to initialize Wayland!");
  }
  if (initXKB(cs) == ANVLK_SUCCESS)
  {
    LOG::INFO(cs.logCtx, logger::LogStyle::COLOR_BOLD, "Successfully initialized XKB!");
  }
  else
  {
    LOG::ERROR(cs.logCtx, "Failed to initialize XKB!");
  }

  logger::switchCtx(cs.logCtx, anvlk::logger::LogCategory::CONFIG);

  anvlk::cfg::ConfigLoader loader(cs.logCtx, cs.homeDir);
  cs.userConfig = loader.load();

  logger::switchCtx(cs.logCtx, anvlk::logger::LogCategory::FREETYPE);

  anvlk::freetype::FreeTypeHandler freeType(cs.logCtx, cs.userConfig.font.path);

  cs.freeTypeState.ftFace    = freeType.face();
  cs.freeTypeState.ftLibrary = freeType.library();

  logger::switchCtx(cs.logCtx, anvlk::logger::LogCategory::SHADERS);

  cs.initShaderManager();

  if (!cs.shaderManagerPtr->isValid())
  {
    std::exit(EXIT_FAILURE);
  }

  cs.logCtx.resetContext();

  wl_surface_commit(cs.wlSurface);

  while (!cs.pamState.authState.authSuccess && wl_display_dispatch(cs.wlDisplay) != -1)
  {
    cs.pamState.authState.authFailed = false;
    if (!cs.sessionLock.surfaceCreated)
    {
      wl::initiateSessionLock(cs);
    }

    render::renderLockScreen(cs);
  }

  cs.logCtx.resetContext();
  wl::unlockAndDestroySessionLock(cs);
  cs.destroyEGL();
  cs.disconnectWLDisplay();
  // freetype gets destroyed via destructor

  LOG::INFO(cs.logCtx, "Cleanup job done. Exiting Anvilock...");

  return ANVLK_SUCCESS;
}
