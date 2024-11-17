#ifndef XDG_SURFACE_HANDLER_H
#define XDG_SURFACE_HANDLER_H

#include "config.h"
#include "egl.h"
#include "log.h"
#include "surface_colors.h"
#include <EGL/egl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client.h>
#include <wayland-egl.h>

// Function for rendering the lock screen
void render_lock_screen(struct client_state* state)
{
  // Bind the context to ensure it's current
  if (!eglMakeCurrent(state->egl_display, state->egl_surface, state->egl_surface,
                      state->egl_context))
  {
    log_message(LOG_LEVEL_ERROR, "Failed to make EGL context current for rendering");
    return;
  }

  render_triangle(state);

  eglSwapBuffers(state->egl_display, state->egl_surface);
}

static void xdg_surface_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial)
{
  struct client_state* state = data;
  xdg_surface_ack_configure(xdg_surface, serial);

  // Ensure EGL and Wayland surface setup is ready before binding the context
  if (state->egl_display && state->egl_surface && state->egl_context)
  {
    // Check if the current EGL context and surface match the ones we're trying to use
    if (eglGetCurrentContext() != state->egl_context ||
        eglGetCurrentSurface(EGL_DRAW) != state->egl_surface)
    {
      if (!eglMakeCurrent(state->egl_display, state->egl_surface, state->egl_surface,
                          state->egl_context))
      {
        log_message(LOG_LEVEL_ERROR, "Failed to make EGL context current");
        return;
      }
    }

    // Render the lock screen (if not done in another function) and commit
    render_lock_screen(state); // Assuming render_lock_screen calls eglSwapBuffers

    state->session_lock.surface_dirty = true;

    // Surface commit to ensure Wayland knows of the new state
    wl_surface_commit(state->wl_surface);
    if (!eglSwapBuffers(state->egl_display, state->egl_surface)) {
        log_message(LOG_LEVEL_ERROR, "Failed to swap EGL buffers");
    }
  }
  else
  {
    // If EGL resources are not ready, defer processing
    log_message(LOG_LEVEL_ERROR, "EGL display or surfaces not ready in xdg_surface_configure... Waiting ...");
  }
}

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_configure,
};

#endif
