#include <anvilock/include/utils/Assert.hpp>
#include <anvilock/include/wayland/session-lock/SessionLockHandler.hpp>

namespace anvlk::wl
{

void handleLocked(types::VPtr data, types::ext::SessionLockObjV1_* /*lock*/)
{
  auto& state           = *static_cast<ClientState*>(data);
  state.pamState.locked = true;
}

void handleFinished(types::VPtr data, types::ext::SessionLockObjV1_* /*lock*/)
{
  auto& state = *static_cast<ClientState*>(data);
  logger::switchCtx(state.logCtx, logger::LogCategory::SESSION_LOCK);
  LOG::ERROR(state.logCtx, "Failed to lock session -- is another lockscreen running?");
  logger::resetCtx(state.logCtx);
  std::exit(2);
}

void handleSurfaceConfigure(types::VPtr data, types::ext::SessionLockSurfaceV1_* lockSurface,
                            u32 serial, u32 width, u32 height)
{
  auto& state              = *static_cast<ClientState*>(data);
  state.outputState.width  = width;
  state.outputState.height = height;

  ext_session_lock_surface_v1_ack_configure(lockSurface, serial);
  state.sessionLock.surfaceDirty = true;

  render::renderLockScreen(state);
}

void createLockSurface(ClientState& state)
{
  logger::switchCtx(state.logCtx, logger::LogCategory::SESSION_LOCK);
  state.wlSurface = wl_compositor_create_surface(state.wlCompositor);
  ANVLK_ASSERT(state.wlSurface != nullptr);

  if (!state.outputState.wlOutput)
  {
    LOG::ERROR(state.logCtx, "No output available for lock surface");
    logger::resetCtx(state.logCtx);
    return;
  }

  state.sessionLock.lockSurface = ext_session_lock_v1_get_lock_surface(
    state.sessionLock.lockObj, state.wlSurface, state.outputState.wlOutput);
  assert(state.sessionLock.extSessionLockSurface);

  ext_session_lock_surface_v1_add_listener(state.sessionLock.lockSurface,
                                           &kSessionLockSurfaceListener, &state);

  render::initEGL(state);

  wl_keyboard_add_listener(state.wlKeyboard, &kKeyboardListener, &state);
  wl_pointer_add_listener(state.wlPointer, &kPointerListener, &state);

  state.sessionLock.surfaceCreated = true;
  logger::resetCtx(state.logCtx);
}

void initiateSessionLock(ClientState& state)
{
  state.sessionLock.lockObj = ext_session_lock_manager_v1_lock(state.sessionLock.lockManager);
  ANVLK_ASSERT(state.sessionLock.lockObj != nullptr);

  ext_session_lock_v1_add_listener(state.sessionLock.lockObj, &kSessionLockListener, &state);

  createLockSurface(state);
  render::renderLockScreen(state);
}

void unlockAndDestroySessionLock(ClientState& state)
{
  logger::switchCtx(state.logCtx, logger::LogCategory::SESSION_LOCK);
  if (state.sessionLock.lockObj)
  {
    ext_session_lock_v1_unlock_and_destroy(state.sessionLock.lockObj);
    state.sessionLock.lockObj = nullptr;
    LOG::INFO(state.logCtx, "Session unlocked and lock object destroyed.");
  }
  logger::resetCtx(state.logCtx);
}

} // namespace anvlk::wl
