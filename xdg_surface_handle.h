#ifndef XDG_SURFACE_HANDLER_H
#define XDG_SURFACE_HANDLER_H

#include "client_state.h"
#include "log.h"
#include "surface_colors.h"
#include "unicode.h"
#include <string.h>
#include <time.h> // Include the time header for getting current time
#include <wayland-client.h>

#define DOT_RADIUS  6  // Radius for the dots
#define CHAR_HEIGHT 20 // Height of characters
#define CHAR_WIDTH  10 // Width of characters

float pulse_factor = 1.5f;

static struct wl_buffer* draw_lock_screen(struct client_state* state, const char* message)
{
  const int width  = state->output_state.width;
  const int height = state->output_state.height;
  int       stride = width * 4;
  int       size   = stride * height;

  int fd = allocate_shm_file(size);
  if (fd == -1)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to allocate shared memory file");
    return NULL;
  }

  uint32_t* data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED)
  {
    log_message(LOG_LEVEL_ERROR, "Memory mapping failed: %s", strerror(errno));
    close(fd);
    return NULL;
  }

  struct wl_shm_pool* pool = wl_shm_create_pool(state->wl_shm, fd, size);
  if (!pool)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create SHM pool: %s", strerror(errno));
    munmap(data, size);
    close(fd);
    return NULL;
  }

  struct wl_buffer* buffer =
    wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
  if (!buffer)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create buffer: %s", strerror(errno));
    wl_shm_pool_destroy(pool);
    munmap(data, size);
    close(fd);
    return NULL;
  }

  // Draw background
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      data[y * width + x] = GRUVBOX_BG; // Set all pixels to background color
    }
  }

  // Draw time
  time_t     t       = time(NULL);
  struct tm* tm_info = localtime(&t);
  char       time_str[20];
  strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);
  int      time_x     = (width - strlen(time_str) * CHAR_WIDTH) / 2; // Center the time
  int      time_y     = 50;                                          // Position for the time
  uint32_t time_color = GRUVBOX_BLUE;

  // Draw time characters
  for (size_t i = 0; time_str[i] != '\0'; i++)
  {
    draw_char(data, time_x + (i * CHAR_WIDTH), time_y, time_str[i], time_color, width);
  }

  // Draw username
  const char* username       = state->username; // Assuming state->username contains the user's name
  int         username_x     = (width - strlen(username) * CHAR_WIDTH) / 2; // Center the username
  int         username_y     = 30; // Position for the username
  uint32_t    username_color = GRUVBOX_WHITE;

  // Draw username characters
  for (size_t i = 0; username[i] != '\0'; i++)
  {
    draw_char(data, username_x + (i * CHAR_WIDTH), username_y, username[i], username_color, width);
  }

  // Draw password prompt box
  int box_width  = 300;
  int box_height = 50;
  int box_x      = (width - box_width) / 2;
  int box_y = (height - box_height) / 2 + 20; // Adjust box_y to fit below the username and time

  // Draw password box background
  for (int y = box_y; y < box_y + box_height; ++y)
  {
    for (int x = box_x; x < box_x + box_width; ++x)
    {
      data[y * width + x] = GRUVBOX_DARK1; // Darker gray for password box
    }
  }

  // Draw the password input (as dots) with pulse effect
  for (int i = 0; i < state->password_index; i++)
  {
    uint32_t color  = GRUVBOX_BLUE;          // Color for password dots
    int      char_x = box_x + 10 + (i * 20); // Assuming a fixed-width font of 20 pixels

    // Draw a filled circle to represent a dot with pulse effect
    for (int dy = -DOT_RADIUS; dy <= DOT_RADIUS; dy++)
    {
      for (int dx = -DOT_RADIUS; dx <= DOT_RADIUS; dx++)
      {
        if (dx * dx + dy * dy <= (DOT_RADIUS + pulse_factor) * (DOT_RADIUS + pulse_factor))
        {
          if (char_x + dx >= box_x && char_x + dx < box_x + box_width &&
              box_y + (box_height / 2) + dy >= box_y &&
              box_y + (box_height / 2) + dy < box_y + box_height)
          {
            data[(box_y + (box_height / 2) + dy) * width + (char_x + dx)] = color;
          }
        }
      }
    }
  }

  // Draw the authentication failed message if provided
  if (message != NULL)
  {
    int message_x = (width - strlen(message) * CHAR_WIDTH) / 2; // Center the message
    int message_y = box_y + box_height + 10;                    // Below the password box
    // Draw each character of the message
    for (int i = 0; message[i] != '\0'; i++)
    {
      uint32_t color = GRUVBOX_RED; // Red for error message
      draw_char(data, message_x + (i * CHAR_WIDTH), message_y, message[i], color, width);
    }
  }

  munmap(data, size);
  wl_shm_pool_destroy(pool);
  close(fd);
  return buffer;
}

static void xdg_surface_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial)
{
  struct client_state* state = data;
  xdg_surface_ack_configure(xdg_surface, serial);

  struct wl_buffer* buffer = draw_lock_screen(state, NULL);
  if (!buffer)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create buffer for lock screen");
    return;
  }

  wl_surface_attach(state->wl_surface, buffer, 0, 0);
  wl_surface_commit(state->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_configure,
};

#endif
