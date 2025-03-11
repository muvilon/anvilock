#ifndef WL_BUF_HANDLER_H
#define WL_BUF_HANDLER_H

#include <wayland-client.h>

static void wl_buffer_release(void* data, struct wl_buffer* wl_buffer)
{
  /* Sent by the compositor when it's no longer using this buffer */
  wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
  .release = wl_buffer_release,
};

#endif
