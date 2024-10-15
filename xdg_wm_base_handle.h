#ifndef XDG_WM_BASE_HANDLER_H
#define XDG_WM_BASE_HANDLER_H

#include <wayland-client.h>

static void xdg_wm_base_ping(void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial)
{
  xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
  .ping = xdg_wm_base_ping,
};

#endif
