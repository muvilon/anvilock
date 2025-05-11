#ifndef ANVLK_FREETYPE_STRUCT_HPP
#define ANVLK_FREETYPE_STRUCT_HPP

#include <freetype/freetype.h>
#include <ft2build.h>
#include FT_FREETYPE_H

struct FreeTypeState
{
  FT_Library ftLibrary;
  FT_Face    ftFace;
};

#endif
