#ifndef FREETYPE_H
#define FREETYPE_H

#include "config.h"
#include "log.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#define DOT_RADIUS  6
#define CHAR_HEIGHT 20
#define CHAR_WIDTH  10

FT_Library ft_library;
FT_Face    ft_face;

static int init_freetype()
{
  if (!load_config())
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
  log_message(LOG_LEVEL_INFO, "FreeType2 initialized => font loaded successfully");
  return 1;
}

#endif
