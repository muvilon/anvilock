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

// Function declarations
static uint32_t blend_colors(uint32_t bg, uint32_t fg, unsigned char alpha);

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

static uint32_t blend_colors(uint32_t bg, uint32_t fg, unsigned char alpha)
{
  uint32_t bg_r = (bg >> 16) & 0xFF;
  uint32_t bg_g = (bg >> 8) & 0xFF;
  uint32_t bg_b = bg & 0xFF;

  uint32_t fg_r = (fg >> 16) & 0xFF;
  uint32_t fg_g = (fg >> 8) & 0xFF;
  uint32_t fg_b = fg & 0xFF;

  uint32_t r = (alpha * fg_r + (255 - alpha) * bg_r) / 255;
  uint32_t g = (alpha * fg_g + (255 - alpha) * bg_g) / 255;
  uint32_t b = (alpha * fg_b + (255 - alpha) * bg_b) / 255;

  return (r << 16) | (g << 8) | b;
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

    // Surface commit to ensure Wayland knows of the new state
    wl_surface_commit(state->wl_surface);
  }
  else
  {
    // If EGL resources are not ready, defer processing
    log_message(LOG_LEVEL_ERROR, "EGL display or surfaces not ready in xdg_surface_configure");
  }
}

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_configure,
};

#endif
