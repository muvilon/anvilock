#pragma once

#include <anvilock/include/Types.hpp>
#include <cassert>
#include <wayland-client.h>

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/wayland/seats/KeyboardHandler.hpp>
#include <anvilock/include/wayland/seats/PointerHandler.hpp>
#include <anvilock/include/wayland/seats/SeatHandler.hpp>

namespace anvlk::wl
{

class SessionLockHandler
{
public:
  static void initiate(ClientState& state)
  {
    state.sessionLock.lockObj = ext_session_lock_manager_v1_lock(state.sessionLock.lockManager);
    assert(state.sessionLock.extSessionLock);

    ext_session_lock_v1_add_listener(state.sessionLock.lockObj, &kSessionLockListener, &state);

    createLockSurface(state);
    //renderLockScreen(state);
  }

  static void unlockAndDestroy(ClientState& state)
  {
    if (state.sessionLock.lockObj)
    {
      ext_session_lock_v1_unlock_and_destroy(state.sessionLock.lockObj);
      state.sessionLock.lockObj = nullptr;
      logger::log(logger::LogLevel::Info, state.logCtx,
                  "Session unlocked and lock object destroyed.");
    }
  }

private:
  static void createLockSurface(ClientState& state)
  {
    state.wlSurface = wl_compositor_create_surface(state.wlCompositor);
    assert(state.wlSurface);

    if (!state.outputState.wlOutput)
    {
      logger::log(logger::LogLevel::Error, state.logCtx, "No output available for lock surface");
      return;
    }

    state.sessionLock.lockSurface = ext_session_lock_v1_get_lock_surface(
      state.sessionLock.lockObj, state.wlSurface, state.outputState.wlOutput);
    assert(state.sessionLock.extSessionLockSurface);

    ext_session_lock_surface_v1_add_listener(state.sessionLock.lockSurface, &kSurfaceListener,
                                             &state);

    //initEGL(state);

    wl_keyboard_add_listener(state.wlKeyboard, &kKeyboardListener, &state);
    wl_pointer_add_listener(state.wlPointer, &kPointerListener, &state);

    state.sessionLock.surfaceCreated = true;
  }

  static void handleLocked(types::VPtr data, types::ext::SessionLockObjV1_* /*lock*/)
  {
    auto& state      = *static_cast<ClientState*>(data);
    state.pam.locked = true;
  }

  static void handleFinished(types::VPtr data, types::ext::SessionLockObjV1_* /*lock*/)
  {
    auto& state = *static_cast<ClientState*>(data);
    anvlk::logger::log(logger::LogLevel::Error, state.logCtx,
                       "Failed to lock session -- is another lockscreen running?");
    std::exit(2);
  }

  static void handleSurfaceConfigure(types::VPtr                        data,
                                     types::ext::SessionLockSurfaceV1_* lockSurface, u32 serial,
                                     u32 width, u32 height)
  {

    auto& state              = *static_cast<ClientState*>(data);
    state.outputState.width  = width;
    state.outputState.height = height;

    ext_session_lock_surface_v1_ack_configure(lockSurface, serial);
    state.sessionLock.surfaceDirty = true;

    //renderLockScreen(state);
  }

  inline static const ext_session_lock_v1_listener kSessionLockListener{.locked   = handleLocked,
                                                                        .finished = handleFinished};

  inline static const ext_session_lock_surface_v1_listener kSurfaceListener{
    .configure = handleSurfaceConfigure};
};

} // namespace anvlk::wl
