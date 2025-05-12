#include <anvilock/src/Anvilock.hpp>

using namespace anvlk;

auto main() -> int
{
  ClientState cs;
  cs.pamState.authState.authSuccess = false;

  setHomeDir(cs);
  ANVLK_ASSERT(cs.homeDir.empty() != true);
  auto usernameOpt = utils::getCurrentUsername();
  ANVLK_ASSERT(usernameOpt->empty() != true);

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
  LOG::INFO(cs.logCtx, logger::LogStyle::COLOR_BOLD_UNDERLINE, "Final log file located at '{}'",
            cs.logCtx.logFilePath.string());

  return ANVLK_SUCCESS;
}
