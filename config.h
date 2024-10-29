#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>

static char* font_path = NULL;
char         errbuf[200];

// 1 - Success, 0 - Failure
static int load_config(const char* config_file_path)
{
  FILE* config_file = fopen(config_file_path, "r");
  if (config_file == NULL)
  {
    fprintf(stderr, "Failed to open config file: %s\n", config_file_path);
    return 0;
  }

  toml_table_t* config = toml_parse_file(config_file, errbuf, sizeof(errbuf));
  fclose(config_file);

  if (!config)
  {
    fprintf(stderr, "Failed to parse config file: %s\n", errbuf);
    return 0;
  }

  toml_table_t* font_table = toml_table_in(config, "font");
  if (!font_table)
  {
    fprintf(stderr, "Failed to find font table in config file: %s\n", config_file_path);
    return 0;
  }

  // Temporary string to store the font path
  char* font_path_str;

  if (toml_rtos(toml_raw_in(font_table, "path"), &font_path_str) == -1)
  {
    fprintf(stderr, "Failed to find font path in config file: %s\n", config_file_path);
    return 0;
  }

  font_path = strdup(font_path_str);

  // Free the TOML and temp string memory
  free((void*)font_path_str);
  toml_free(config);
  return 1;
}

static const char* get_font_path() { return font_path; }

static void free_config()
{
  if (font_path != NULL)
  {
    free(font_path);
  }
  font_path = NULL;
}

#endif
