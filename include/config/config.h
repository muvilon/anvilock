#ifndef CONFIG_H
#define CONFIG_H

#include "../../toml/toml.h"
#include "../client_state.h"
#include "../log.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_LOAD_SUCCESS 1
#define CONFIG_LOAD_FAIL    0

// Global config instance
static TOMLConfig global_config = {NULL, NULL, NULL, NULL, NULL};

// Buffer to hold config file path
static char _config_path[256];

// Returns the config file path
static const char* get_config_file_path(void)
{
  const char* home = getenv("HOME");
  if (!home)
  {
    log_message(LOG_LEVEL_ERROR, "HOME environment variable not set.");
    return NULL;
  }

  snprintf(_config_path, sizeof(_config_path), "%s/.config/anvilock/config.toml", home);
  return _config_path;
}

// Parses a float array from a TOML table
static int get_toml_float_array(toml_table_t* table, const char* key, float* x, float* y)
{
  toml_array_t* arr = toml_array_in(table, key);
  if (!arr || toml_array_nelem(arr) != 2)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Key '%s' must be an array of two floats.", key);
    return CONFIG_LOAD_FAIL;
  }

  toml_datum_t x_datum = toml_double_at(arr, 0);
  toml_datum_t y_datum = toml_double_at(arr, 1);

  if (!x_datum.ok || !y_datum.ok)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Key '%s' contains invalid float values.", key);
    return CONFIG_LOAD_FAIL;
  }

  *x = (float)x_datum.u.d;
  *y = (float)y_datum.u.d;

  return CONFIG_LOAD_SUCCESS;
}

// Helper function to read a string from a TOML table
static char* get_toml_string(toml_table_t* table, const char* key)
{
  char* value = NULL;
  if (toml_rtos(toml_raw_in(table, key), &value) == -1)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Failed to find key: %s", key);
    return NULL;
  }
  return value;
}

// Loads configuration from TOML file
static int load_config(void)
{
  const char* config_path = get_config_file_path();
  if (!config_path)
  {
    return CONFIG_LOAD_FAIL;
  }

  FILE* config_file = fopen(config_path, "r");
  if (!config_file)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Failed to open config file: %s", config_path);
    return CONFIG_LOAD_FAIL;
  }

  char          errbuf[200];
  toml_table_t* root = toml_parse_file(config_file, errbuf, sizeof(errbuf));
  fclose(config_file);

  if (!root)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Parsing failed: %s", errbuf);
    return CONFIG_LOAD_FAIL;
  }

  // Parse sections
  toml_table_t* font_table        = toml_table_in(root, "font");
  toml_table_t* bg_table          = toml_table_in(root, "bg");
  toml_table_t* time_format_table = toml_table_in(root, "time");
  toml_table_t* debug_table       = toml_table_in(root, "debug");
  toml_table_t* time_box_table    = toml_table_in(root, "time_box");

  if (!font_table || !bg_table || !time_format_table || !debug_table)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Missing required sections.");
    toml_free(root);
    return CONFIG_LOAD_FAIL;
  }

  // Assign values
  global_config.font_path        = get_toml_string(font_table, "path");
  global_config.bg_name          = get_toml_string(bg_table, "name");
  global_config.bg_path          = get_toml_string(bg_table, "path");
  global_config.time_format      = get_toml_string(time_format_table, "time_format");
  global_config.debug_log_enable = get_toml_string(debug_table, "debug_log_enable");

  float texcoords[4][2] = {
    {0.0f, 0.0f}, // Top left
    {1.0f, 0.0f}, // Top right
    {0.0f, 1.0f}, // Bottom left
    {1.0f, 1.0f}  // Bottom right
  };

  // Load time box vertices
  const char* keys[] = {"top_left", "top_right", "bottom_left", "bottom_right"};
  for (int i = 0; i < 4; i++)
  {
    if (get_toml_float_array(time_box_table, keys[i], &global_config.time_box_vertices[i].x,
                             &global_config.time_box_vertices[i].y) == CONFIG_LOAD_FAIL)
    {
      toml_free(root);
      return CONFIG_LOAD_FAIL;
    }

    global_config.time_box_vertices[i].u = texcoords[i][0];
    global_config.time_box_vertices[i].v = texcoords[i][1];
  }

  toml_free(root);
  return CONFIG_LOAD_SUCCESS;
}

// Getter functions
static const TOMLConfig* get_config(void) { return &global_config; }

// Free dynamically allocated strings
static void free_config(void)
{
  free(global_config.font_path);
  free(global_config.bg_name);
  free(global_config.bg_path);
  free(global_config.debug_log_enable);
  free(global_config.time_format);

  memset(&global_config, 0, sizeof(TOMLConfig));
}

#endif // CONFIG_H
