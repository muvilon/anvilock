#pragma once

#define _POSIX_C_SOURCE 200809L

#include "../include/client_state.h"
#include "../include/config/config.h"
#include "../include/freetype/freetype.h"
#include "../include/graphics/shaders.h"
#include "../include/log.h"
#include "../include/pam/pam.h"
#include "../include/wayland/session_lock_handle.h"
#include "../include/wayland/wl_registry_handle.h"
#include "../include/wayland/xdg_surface_handle.h"

static int initialize_wayland(struct client_state* state)
{
  state->wl_display = wl_display_connect(NULL);
  if (!state->wl_display)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to connect to Wayland display\n");
    return -1;
  }

  state->wl_registry = wl_display_get_registry(state->wl_display);
  wl_registry_add_listener(state->wl_registry, &wl_registry_listener, state);
  wl_display_roundtrip(state->wl_display); // Get Wayland objects

  state->wl_surface  = wl_compositor_create_surface(state->wl_compositor);
  state->xdg_surface = xdg_wm_base_get_xdg_surface(state->xdg_wm_base, state->wl_surface);
  xdg_surface_add_listener(state->xdg_surface, &xdg_surface_listener, state);

  state->xdg_toplevel = xdg_surface_get_toplevel(state->xdg_surface);
  xdg_toplevel_set_title(state->xdg_toplevel, "Anvilock");

  return 0;
}

static int initialize_xkb(struct client_state* state)
{
  state->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (!state->xkb_context)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to initialize XKB context\n");
    return -1;
  }

  state->xkb_keymap =
    xkb_keymap_new_from_names(state->xkb_context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!state->xkb_keymap)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create XKB keymap\n");
    return -1;
  }

  state->xkb_state = xkb_state_new(state->xkb_keymap);
  if (!state->xkb_state)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create XKB state\n");
    return -1;
  }

  return 0;
}

static int initialize_freetype(struct client_state* state)
{
  int ft = init_freetype();
  if (ft != 0)
  {
    return 0;
  }
  log_message(LOG_LEVEL_ERROR, "Initializing FreeType2 was unsuccessful.");
  return -1;
}

static int initialize_configs(struct client_state* state)
{
  int loadConfig = load_config();

  if (loadConfig == CONFIG_LOAD_SUCCESS)
  {
    state->global_config = *get_config();
  }

  const char* background_path =
    state->global_config.bg_path; // Get the background path from the loaded TOML config
  if (!background_path || background_path == NULL)
  {
    log_message(LOG_LEVEL_ERROR, "Background path not found in config");
    return -1;
  }

  // Check if the file exists and can be accessed
  FILE* file = fopen(background_path, "r");
  if (!file)
  {
    log_message(LOG_LEVEL_ERROR, "Background file does not exist or cannot be accessed: %s",
                background_path);
    return -1;
  }

  // Close the file after checking
  fclose(file);

  log_message(LOG_LEVEL_TRACE, "Found bg path through config.toml ==> %s",
              state->global_config.bg_path);

  const char* debug_option_str = global_config.debug_log_enable;
  if (!debug_option_str)
  {
    log_message(LOG_LEVEL_ERROR, "Debug option not set");
    return -1;
  }

  return 0;
}

// Check if the shader file exists
static void shader_exist(const char* relfilepath, const char* shader_runtime_dir)
{
  char* abs_filepath = ANVIL_SAFE_STR_JOIN(shader_runtime_dir, relfilepath);
  FILE* file         = fopen(abs_filepath, "r");
  if (!file)
  {
    log_message(LOG_LEVEL_ERROR, "[SHADERS] Failed to open shader file: %s", abs_filepath);
    exit(EXIT_FAILURE);
  }
  else
  {
    log_message(LOG_LEVEL_TRACE, "[SHADERS] Preloaded shader '%s' successfully.", relfilepath);
  }
}

// Initialize shaders by checking all of them
static void initialize_shaders(const char* shader_runtime_dir)
{
// Iterate over all shader paths and check if they exist
#define X(name, path)                                                 \
  log_message(LOG_LEVEL_DEBUG, "[SHADERS] Loading shader %s.", path); \
  shader_exist(path, shader_runtime_dir);

  SHADER_PATHS // Expands to all shaders
#undef X

    log_message(LOG_LEVEL_INFO, "[SHADERS] Found and initialized all shaders.");
}

static void cleanup(struct client_state* state)
{
  unlock_and_destroy_session_lock(state);
  eglDestroySurface(state->egl_display, state->egl_surface);
  eglDestroyContext(state->egl_display, state->egl_context);
  eglTerminate(state->egl_display);
  wl_display_roundtrip(state->wl_display);
  wl_display_disconnect(state->wl_display);

  FT_Done_Face(ft_face);
  FT_Done_FreeType(ft_library);

  log_message(LOG_LEVEL_TRACE, "Anvilock resources cleanup completed. Exiting...");
}
