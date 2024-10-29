#ifndef XDG_SURFACE_HANDLER_H
#define XDG_SURFACE_HANDLER_H

#include "config.h"
#include "log.h"
#include "surface_colors.h"
#include "unicode.h"
#include <ft2build.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client.h>
#include FT_FREETYPE_H

#define DOT_RADIUS  6
#define CHAR_HEIGHT 20
#define CHAR_WIDTH  10

float pulse_factor = 1.5f;

FT_Library ft_library;
FT_Face    ft_face;

// Function declarations
static int      init_freetype();
static void     render_text(uint32_t* buffer, int buffer_width, int x, int y, const char* text,
                            uint32_t color);
static uint32_t blend_colors(uint32_t bg, uint32_t fg, unsigned char alpha);
static struct wl_buffer* draw_lock_screen(struct client_state* state, const char* message);

// Function implementations
static int init_freetype()
{
  if (!load_config("config.toml"))
  {
    log_message(LOG_LEVEL_ERROR, "Failed to load config file");
    return 0;
  }

  int error = FT_Init_FreeType(&ft_library);
  if (error)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to initialize FreeType");
    return 0;
  }

  error = FT_New_Face(ft_library, font_path, 0, &ft_face);
  if (error)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to load font");
    return 0;
  }

  error = FT_Set_Pixel_Sizes(ft_face, 0, CHAR_HEIGHT);
  if (error)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to set font size");
    return 0;
  }
  log_message(LOG_LEVEL_INFO, "Loaded font with freetype.");
  return 1;
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

static void render_text(uint32_t* buffer, int buffer_width, int x, int y, const char* text,
                        uint32_t color)
{
  FT_GlyphSlot slot = ft_face->glyph;

  for (int n = 0; n < strlen(text); n++)
  {
    if (FT_Load_Char(ft_face, text[n], FT_LOAD_RENDER))
    {
      continue;
    }

    for (int i = 0; i < slot->bitmap.rows; i++)
    {
      for (int j = 0; j < slot->bitmap.width; j++)
      {
        int pos_x = x + j + slot->bitmap_left;
        int pos_y = y + i - slot->bitmap_top + CHAR_HEIGHT;

        if (pos_x < 0 || pos_y < 0 || pos_x >= buffer_width || pos_y >= buffer_width / 4)
        {
          continue;
        }

        unsigned char pixel = slot->bitmap.buffer[i * slot->bitmap.width + j];
        if (pixel > 0)
        {
          uint32_t pixel_color = blend_colors(buffer[pos_y * buffer_width + pos_x], color, pixel);
          buffer[pos_y * buffer_width + pos_x] = pixel_color;
        }
      }
    }

    x += slot->advance.x >> 6;
  }
}

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
      data[y * width + x] = GRUVBOX_BG;
    }
  }

  // Draw time
  time_t     t       = time(NULL);
  struct tm* tm_info = localtime(&t);
  char       time_str[20];
  strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);
  int time_x = (width - strlen(time_str) * CHAR_WIDTH) / 2;
  int time_y = 50;
  render_text(data, width, time_x, time_y, time_str, GRUVBOX_BLUE);

  // Draw username
  const char* username   = state->username;
  int         username_x = (width - strlen(username) * CHAR_WIDTH) / 2;
  int         username_y = 30;
  render_text(data, width, username_x, username_y, username, GRUVBOX_WHITE);

  // Draw password prompt box
  int box_width  = 320;
  int box_height = 50;
  int box_x      = (width - box_width) / 2;
  int box_y      = (height - box_height) / 2 + 20;

  for (int y = box_y; y < box_y + box_height; ++y)
  {
    for (int x = box_x; x < box_x + box_width; ++x)
    {
      data[y * width + x] = GRUVBOX_DARK1;
    }
  }

  // Draw the password input (as dots) with pulse effect
  for (int i = 0; i < state->password_index; i++)
  {
    uint32_t color  = GRUVBOX_BLUE;
    int      char_x = box_x + 10 + (i * 20);

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
    int message_x = (width - strlen(message) * CHAR_WIDTH) / 2;
    int message_y = box_y + box_height + 10;
    render_text(data, width, message_x, message_y, message, GRUVBOX_RED);
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

/* GRACEFUL EXIT */

void fade_out_effect(struct client_state* state)
{
  const int  FADE_STEPS    = 60;       // Number of fade steps
  const long FADE_DELAY_NS = 16667000; // Delay between steps in nanoseconds (60 fps)

  struct timespec ts = {.tv_sec = 0, .tv_nsec = FADE_DELAY_NS};

  // Store the original pulse factor
  float original_pulse_factor = pulse_factor;

  for (int i = FADE_STEPS; i >= 0; i--)
  {
    float alpha = (float)i / FADE_STEPS;

    // Modify the global pulse_factor to create a fading effect
    pulse_factor = original_pulse_factor * alpha;

    // Create a new buffer for this frame using the existing draw_lock_screen function
    struct wl_buffer* buffer = draw_lock_screen(state, NULL);
    if (!buffer)
    {
      log_message(LOG_LEVEL_ERROR, "Failed to create buffer for fade-out effect");
      return;
    }

    // Attach and commit the new buffer
    wl_surface_attach(state->wl_surface, buffer, 0, 0);
    wl_surface_damage_buffer(state->wl_surface, 0, 0, state->output_state.width,
                             state->output_state.height);
    wl_surface_commit(state->wl_surface);

    // Wait for the next frame
    nanosleep(&ts, NULL);

    // Process any pending events
    wl_display_dispatch_pending(state->wl_display);

    // Destroy the buffer after use
    wl_buffer_destroy(buffer);
  }

  // Restore the original pulse factor
  pulse_factor = original_pulse_factor;
}

#endif
