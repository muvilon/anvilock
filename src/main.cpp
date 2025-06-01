#include <anvilock/src/Anvilock.hpp>

auto main() -> int
{
  ClientState cs;

  setHomeDir(cs);
  ANVLK_ASSERT(cs.homeDir.empty() != true);
  auto usernameOpt = utils::getCurrentUsername();
  ANVLK_ASSERT(usernameOpt->empty() != true);

  cs.pamState.username = *usernameOpt;

  LOG::INFO(cs.logCtx, "Session Lock JOB requested by user @: '{}'", cs.pamState.username);

  ANVLK_ASSERT_EQ(initWayland(cs), ANVLK_SUCCESS);
  LOG::INFO(cs.logCtx, logger::LogStyle::COLOR_BOLD, "Successfully initialized Wayland!!");
  ANVLK_ASSERT_EQ(initXKB(cs), ANVLK_SUCCESS);
  LOG::INFO(cs.logCtx, logger::LogStyle::COLOR_BOLD, "Successfully initialized XKB!");

  logger::switchCtx(cs.logCtx, logger::LogCategory::CONFIG);

  anvlk::cfg::ConfigLoader cfgLoader(cs.logCtx, cs.homeDir);
  cs.userConfig = cfgLoader.load();

  logger::switchCtx(cs.logCtx, logger::LogCategory::FREETYPE);

  anvlk::freetype::FreeTypeHandler freeType(cs.logCtx, cs.userConfig.font.path);

  cs.freeTypeState.ftFace    = freeType.face();
  cs.freeTypeState.ftLibrary = freeType.library();

  logger::switchCtx(cs.logCtx, logger::LogCategory::SHADERS);

  cs.initShaderManager();

  ANVLK_ASSERT(cs.shaderManagerPtr->isValid() == true);

  cs.logCtx.resetContext();

  // all init jobs done, now we commit surface to begin session lock
  // and session-lock surface re-rendering (dirtying) with EGL
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

  sessionCleanup(cs);

  return ANVLK_SUCCESS;
}
