
#include "anvilock/include/Log.hpp"
#include <anvilock/include/LogMacros.hpp>
#include <anvilock/include/renderer/EGL.hpp>
#include <anvilock/include/wayland/xdg/SurfaceHandler.hpp>

namespace anvlk::wl
{

static void handleXdgSurfaceConfigure(types::VPtr                              data,
                                      anvlk::types::wayland::xdg::XDGSurface_* xdgSurface,
                                      u32                                      serial)
{
  auto* state = static_cast<ClientState*>(data);
  logger::switchCtx(state->logCtx, logger::LogCategory::XDG_SURFACE);
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
        LOG::ERROR(state->logCtx, "Failed to make EGL context current.");
        return;
      }
    }

    render::renderLockScreen(*state);

    state->sessionLock.surfaceDirty = true;

    wl_surface_commit(state->wlSurface);

    if (!eglSwapBuffers(state->eglDisplay, state->eglSurface))
    {
      LOG::ERROR(state->logCtx, "Failed to swap EGL buffers");
    }
  }
  else
  {
    LOG::WARN(state->logCtx,
              "EGL display or surfaces not ready in xdg_surface_configure... Waiting ...");
  }
  logger::resetCtx(state->logCtx);
}

// Listener instance
const xdg_surface_listener kXdgSurfaceListener{
  .configure = handleXdgSurfaceConfigure,
};

} // namespace anvlk::wl
