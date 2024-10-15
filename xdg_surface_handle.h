#ifndef XDG_SURFACE_HANDLER_H
#define XDG_SURFACE_HANDLER_H

#include <wayland-client.h>

static struct wl_buffer* draw_frame(struct client_state* state)
{
  const int width = 640, height = 480;
  int       stride = width * 4;
  int       size   = stride * height;

  int fd = allocate_shm_file(size);
  if (fd == -1)
  {
    return NULL;
  }

  uint32_t* data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED)
  {
    close(fd);
    return NULL;
  }

  struct wl_shm_pool* pool = wl_shm_create_pool(state->wl_shm, fd, size);
  struct wl_buffer*   buffer =
    wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
  wl_shm_pool_destroy(pool);
  close(fd);

  /* Draw checkerboxed background */
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      if ((x + y / 8 * 8) % 16 < 8)
        data[y * width + x] = 0xFF666666;
      else
        data[y * width + x] = 0xFFEEEEEE;
    }
  }

  munmap(data, size);
  wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
  return buffer;
}

static void xdg_surface_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial)
{
  struct client_state* state = data;
  xdg_surface_ack_configure(xdg_surface, serial);

  struct wl_buffer* buffer = draw_frame(state);
  wl_surface_attach(state->wl_surface, buffer, 0, 0);
  wl_surface_commit(state->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_configure,
};

#endif
