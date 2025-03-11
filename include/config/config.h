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

// Structure to hold configuration values

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
//
// RETURNS 1 on SUCCESS and 0 on FAIL
static int load_config(void)
{
  const char* config_path = get_config_file_path();
  if (!config_path)
  {
    return 0;
  }

  FILE* config_file = fopen(config_path, "r");
  if (!config_file)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Failed to open config file: %s", config_path);
    return 0;
  }

  char          errbuf[200];
  toml_table_t* root = toml_parse_file(config_file, errbuf, sizeof(errbuf));
  fclose(config_file);

  if (!root)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Parsing failed: %s", errbuf);
    return 0;
  }

  // Parse sections
  toml_table_t* font_table        = toml_table_in(root, "font");
  toml_table_t* bg_table          = toml_table_in(root, "bg");
  toml_table_t* time_format_table = toml_table_in(root, "time");
  toml_table_t* debug_table       = toml_table_in(root, "debug");

  if (!font_table || !bg_table || !time_format_table || !debug_table)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Missing required sections.");
    toml_free(root);
    return 0;
  }

  // Assign values
  global_config.font_path        = get_toml_string(font_table, "path");
  global_config.bg_name          = get_toml_string(bg_table, "name");
  global_config.bg_path          = get_toml_string(bg_table, "path");
  global_config.time_format      = get_toml_string(time_format_table, "time_format");
  global_config.debug_log_enable = get_toml_string(debug_table, "debug_log_enable");

  toml_free(root);
  return 1;
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
