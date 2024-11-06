#ifndef CONFIG_H
#define CONFIG_H

#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>

#define CONFIG_FILE_PATH                                                                           \
  ({                                                                                               \
    const char* home = getenv("HOME");                                                             \
    home ? (snprintf(_config_path, sizeof(_config_path), "%s/.config/anvilock/config.toml", home), \
            _config_path)                                                                          \
         : (log_message(LOG_LEVEL_ERROR, "HOME environment variable not set."), NULL);             \
  })

// Buffer to hold config file path
static char  _config_path[256];
static char* font_path = NULL;
static char  errbuf[200];

// 1 - Success, 0 - Failure
static int load_config()
{
  const char* config_path = CONFIG_FILE_PATH;
  if (!config_path)
  {
    return 0;
  }

  FILE* config_file = fopen(config_path, "r");
  if (config_file == NULL)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Failed to open config file: %s", config_path);
    return 0;
  }

  toml_table_t* config = toml_parse_file(config_file, errbuf, sizeof(errbuf));
  fclose(config_file);

  if (!config)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Failed to parse config file: %s", errbuf);
    return 0;
  }

  toml_table_t* font_table = toml_table_in(config, "font");
  if (!font_table)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Failed to find font table in config file: %s",
                config_path);
    toml_free(config);
    return 0;
  }

  // Temporary string to store the font path
  char* font_path_str;
  if (toml_rtos(toml_raw_in(font_table, "path"), &font_path_str) == -1)
  {
    log_message(LOG_LEVEL_ERROR, "[TOML] Failed to find font path in config file: %s", config_path);
    toml_free(config);
    return 0;
  }

  font_path = strdup(font_path_str);

  // Free the TOML and temporary string memory
  free(font_path_str);
  toml_free(config);
  return 1;
}

static const char* get_font_path() { return font_path; }

static void free_config()
{
  if (font_path != NULL)
  {
    free(font_path);
    font_path = NULL;
  }
}

#endif
