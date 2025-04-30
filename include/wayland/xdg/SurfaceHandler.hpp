#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <wayland-client.h>
#include <wayland-egl.h>

namespace anvlk::wl
{

using logL = anvlk::logger::LogLevel;

inline void handleXdgSurfaceConfigure(void* data, xdg_surface* xdgSurface, uint32_t serial)
{
  auto* state = static_cast<ClientState*>(data);
  xdg_surface_ack_configure(xdgSurface, serial);

  if (state->eglDisplay && state->eglSurface && state->eglContext)
  {
    const bool contextMismatch = (eglGetCurrentContext() != state->eglContext ||
                                  eglGetCurrentSurface(EGL_DRAW) != state->eglSurface);

    if (contextMismatch)
    {
      if (!eglMakeCurrent(state->eglDisplay, state->eglSurface, state->eglSurface,
                          state->eglContext))
      {
        anvlk::logger::log(logL::Error, state->logCtx, "Failed to make EGL context current.");
        return;
      }
    }

    //render_lock_screen(*state); // Assuming this takes a ClientState& in modern version

    state->sessionLock.surfaceDirty = true;

    wl_surface_commit(state->wlSurface);
    if (!eglSwapBuffers(state->eglDisplay, state->eglSurface))
    {
      anvlk::logger::log(logL::Error, state->logCtx, "Failed to swap EGL buffers");
    }
  }
  else
  {
    anvlk::logger::log(logL::Warn, state->logCtx,
                       "EGL display or surfaces not ready in xdg_surface_configure... Waiting ...");
  }
}

inline const xdg_surface_listener kXdgSurfaceListener{
  .configure = handleXdgSurfaceConfigure,
};

} // namespace anvlk::wl
