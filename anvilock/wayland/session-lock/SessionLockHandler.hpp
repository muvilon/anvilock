#ifndef ANVLK_WAYLAND_SESSION_LOCK_SESSION_LOCK_HANDLER_HPP
#define ANVLK_WAYLAND_SESSION_LOCK_SESSION_LOCK_HANDLER_HPP

#include <anvilock/Types.hpp>
#include <cassert>
#include <wayland-client.h>

#include <anvilock/ClientState.hpp>
#include <anvilock/LogMacros.hpp>
#include <anvilock/renderer/EGL.hpp>
#include <anvilock/wayland/seats/KeyboardHandler.hpp>
#include <anvilock/wayland/seats/PointerHandler.hpp>
#include <anvilock/wayland/seats/SeatHandler.hpp>

namespace anvlk::wl
{

void handleLocked(types::VPtr data, types::ext::SessionLockObjV1_* /*lock*/);
void handleFinished(types::VPtr data, types::ext::SessionLockObjV1_* /*lock*/);
void handleSurfaceConfigure(types::VPtr data, types::ext::SessionLockSurfaceV1_* lockSurface,
                            u32 serial, u32 width, u32 height);
void createLockSurface(ClientState& state);
void initiateSessionLock(ClientState& state);
void unlockAndDestroySessionLock(ClientState& state);

inline static const ext_session_lock_v1_listener kSessionLockListener{.locked   = handleLocked,
                                                                      .finished = handleFinished};

inline static const ext_session_lock_surface_v1_listener kSessionLockSurfaceListener{
  .configure = handleSurfaceConfigure};

} // namespace anvlk::wl

#endif
