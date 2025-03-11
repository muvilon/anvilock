#ifndef WL_OUTPUT_HANDLE_H
#define WL_OUTPUT_HANDLE_H

#include "../client_state.h"
#include "../log.h"
#include <stdio.h>
#include <wayland-client.h>

// This function will be called whenever output geometry changes
static void handle_output_geometry(void* data, struct wl_output* wl_output, int32_t x, int32_t y,
                                   int32_t physical_width, int32_t physical_height,
                                   int32_t subpixel, const char* make, const char* model,
                                   int32_t transform)
{
  struct client_state* state = data;
  log_message(LOG_LEVEL_INFO, "Output: %s %s @ (%d, %d) [%d x %d] mm", make, model, x, y,
              physical_width, physical_height);
}

// This function will be called whenever output mode changes
static void handle_output_mode(void* data, struct wl_output* wl_output, uint32_t flags,
                               int32_t width, int32_t height, int32_t refresh_rate)
{
  struct client_state* state = data;

  // Update state with the output mode (width, height, refresh rate)
  state->output_state.width        = width;
  state->output_state.height       = height;
  state->output_state.refresh_rate = refresh_rate;

  log_message(LOG_LEVEL_INFO, "Output mode: %dx%d @ %d Hz", width, height, refresh_rate);
}

// This function will be called when the output scale changes
static void handle_output_scale(void* data, struct wl_output* wl_output, int32_t factor)
{
  log_message(LOG_LEVEL_INFO, "Output scale factor: %d", factor);
}

// This function will be called when the compositor sends a "done" event
static void handle_output_done(void* data, struct wl_output* wl_output)
{
  log_message(LOG_LEVEL_INFO, "Output configuration done.");
}

// This function will be called to get the output name
static void handle_output_name(void* data, struct wl_output* wl_output, const char* name)
{
  log_message(LOG_LEVEL_INFO, "Output name: %s", name);
}

// This function will be called to get the output description
static void handle_output_description(void* data, struct wl_output* wl_output,
                                      const char* description)
{
  log_message(LOG_LEVEL_INFO, "Output description: %s", description);
}

// Output listener struct
static const struct wl_output_listener wl_output_listener = {
  .geometry    = handle_output_geometry,
  .mode        = handle_output_mode,
  .done        = handle_output_done,
  .scale       = handle_output_scale,
  .name        = handle_output_name,
  .description = handle_output_description,
};

// Function to register an output object and set up the listener
static void register_output(struct client_state* state, struct wl_registry* registry, uint32_t id,
                            uint32_t version)
{
  // Ensure the wl_output is already bound
  if (state->output_state.wl_output)
  {
    log_message(LOG_LEVEL_ERROR, "Output is already bound. Cannot register output listener again.");
    return;
  }

  // Bind the wl_output and add the listener
  log_message(LOG_LEVEL_INFO, "Registered output with ID: %d", id);
}

#endif // WL_OUTPUT_HANDLE_H
