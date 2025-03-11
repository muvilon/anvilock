#ifndef UNICODE_H
#define UNICODE_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/*
 * THIS HEADER IS NOT USED IN THE PROJECT
 *
 * IDK WHAT TO DO WITH IT
 *
 * WILL BE REMOVED SHORTLY
 *
 */

#define CHAR_HEIGHT 20 // Height of characters
#define CHAR_WIDTH  10 // Width of characters

// UTF-8 Utility Functions

int utf8_last_size(const char* str)
{
  int   len = 0;
  char* pos = strchr(str, '\0');
  while (pos > str)
  {
    --pos;
    ++len;
    if ((*pos & 0xc0) != 0x80)
    {
      return len;
    }
  }
  return 0;
}

size_t utf8_chsize(uint32_t ch)
{
  if (ch < 0x80)
  {
    return 1;
  }
  else if (ch < 0x800)
  {
    return 2;
  }
  else if (ch < 0x10000)
  {
    return 3;
  }
  return 4;
}

size_t utf8_encode(char* str, uint32_t ch)
{
  size_t  len = 0;
  uint8_t first;

  if (ch < 0x80)
  {
    first = 0;
    len   = 1;
  }
  else if (ch < 0x800)
  {
    first = 0xc0;
    len   = 2;
  }
  else if (ch < 0x10000)
  {
    first = 0xe0;
    len   = 3;
  }
  else
  {
    first = 0xf0;
    len   = 4;
  }

  for (size_t i = len - 1; i > 0; --i)
  {
    str[i] = (ch & 0x3f) | 0x80;
    ch >>= 6;
  }

  str[0] = ch | first;
  return len;
}

uint32_t utf8_decode(const char** s)
{
  const unsigned char* p     = (const unsigned char*)*s;
  uint32_t             ch    = 0;
  int                  extra = 0;

  if (*p < 0x80)
  {
    ch    = *p;
    extra = 0;
  }
  else if ((*p & 0xE0) == 0xC0)
  {
    ch    = *p & 0x1F;
    extra = 1;
  }
  else if ((*p & 0xF0) == 0xE0)
  {
    ch    = *p & 0x0F;
    extra = 2;
  }
  else if ((*p & 0xF8) == 0xF0)
  {
    ch    = *p & 0x07;
    extra = 3;
  }
  else
  {
    *s += 1;
    return '?'; // Invalid UTF-8 sequence, return a placeholder
  }

  *s += 1;
  while (extra-- > 0)
  {
    ch = (ch << 6) | (*(*s)++ & 0x3F);
  }

  return ch;
}

int utf8_strlen(const char* s)
{
  int count = 0;
  while (*s)
  {
    utf8_decode(&s); // Advance by one UTF-8 character
    count++;
  }
  return count;
}

static const struct
{
  uint8_t mask;
  uint8_t result;
  int     octets;
} sizes[] = {
  {0x80, 0x00, 1}, {0xE0, 0xC0, 2}, {0xF0, 0xE0, 3},  {0xF8, 0xF0, 4},
  {0xFC, 0xF8, 5}, {0xFE, 0xF8, 6}, {0x80, 0x80, -1},
};

int utf8_size(const char* s)
{
  uint8_t c = (uint8_t)*s;
  for (size_t i = 0; i < sizeof(sizes) / sizeof(*sizes); ++i)
  {
    if ((c & sizes[i].mask) == sizes[i].result)
    {
      return sizes[i].octets;
    }
  }
  return -1;
}

// Bitmap Font for Drawing Characters

static const uint8_t font[128][5] = {
  // Uppercase letters
  ['A'] = {0b000011100, 0b000100010, 0b001111110, 0b010000001, 0b100000001},
  ['B'] = {0b001111100, 0b010000010, 0b001111100, 0b010000010, 0b001111100},
  ['C'] = {0b000111110, 0b001000000, 0b001000000, 0b001000000, 0b000111110},
  ['D'] = {0b001111000, 0b010000100, 0b010000010, 0b010000100, 0b001111000},
  ['E'] = {0b001111110, 0b001000000, 0b001111110, 0b001000000, 0b001111110},
  ['F'] = {0b001111110, 0b001000000, 0b001111110, 0b001000000, 0b001000000},
  ['G'] = {0b000111110, 0b001000000, 0b001001110, 0b001000010, 0b000111110},
  ['H'] = {0b010000010, 0b010000010, 0b011111110, 0b010000010, 0b010000010},
  ['I'] = {0b000011100, 0b000001000, 0b000001000, 0b000001000, 0b000011100},
  ['J'] = {0b000000110, 0b000000010, 0b000000010, 0b010000010, 0b001111100},
  ['K'] = {0b010000010, 0b010001100, 0b010011000, 0b010100100, 0b011000010},
  ['L'] = {0b001000000, 0b001000000, 0b001000000, 0b001000000, 0b001111110},
  ['M'] = {0b010000010, 0b011000110, 0b010101010, 0b010000010, 0b010000010},
  ['N'] = {0b010000010, 0b011000010, 0b010100010, 0b010010010, 0b010001010},
  ['O'] = {0b000111100, 0b001000010, 0b010000001, 0b001000010, 0b000111100},
  ['P'] = {0b001111100, 0b010000010, 0b001111100, 0b001000000, 0b001000000},
  ['Q'] = {0b000111100, 0b001000010, 0b010000001, 0b010000010, 0b000111101},
  ['R'] = {0b001111100, 0b010000010, 0b001111100, 0b010001000, 0b001000100},
  ['S'] = {0b000111110, 0b001000000, 0b000111100, 0b000000010, 0b001111100},
  ['T'] = {0b011111110, 0b000001000, 0b000001000, 0b000001000, 0b000001000},
  ['U'] = {0b010000010, 0b010000010, 0b010000010, 0b010000010, 0b001111100},
  ['V'] = {0b010000010, 0b010000010, 0b010000010, 0b001000100, 0b000111000},
  ['W'] = {0b010000010, 0b010000010, 0b010101010, 0b011000110, 0b010000010},
  ['X'] = {0b010000010, 0b001000100, 0b000111000, 0b001000100, 0b010000010},
  ['Y'] = {0b010000010, 0b001000100, 0b000111000, 0b000001000, 0b000001000},
  ['Z'] = {0b001111110, 0b000000100, 0b000011000, 0b000100000, 0b001111110},

  // Lowercase letters
  ['a'] = {0b000000000, 0b000111100, 0b000000010, 0b000111110, 0b000111110},
  ['b'] = {0b001000000, 0b001111100, 0b001000010, 0b001000010, 0b001111100},
  ['c'] = {0b000000000, 0b000111100, 0b001000000, 0b001000000, 0b000111100},
  ['d'] = {0b000000010, 0b000111110, 0b001000010, 0b001000010, 0b000111110},
  ['e'] = {0b000111100, 0b001000010, 0b001111110, 0b001000000, 0b000111110},
  ['f'] = {0b000001110, 0b000010000, 0b000111110, 0b000010000, 0b000010000},
  ['g'] = {0b000111110, 0b001000010, 0b000111110, 0b000000010, 0b000111100},
  ['h'] = {0b001000000, 0b001111100, 0b001000010, 0b001000010, 0b001000010},
  ['i'] = {0b000000000, 0b000001000, 0b000001000, 0b000001000, 0b000001000},
  ['j'] = {0b000000010, 0b000000000, 0b000000010, 0b000000010, 0b001111100},
  ['k'] = {0b001000000, 0b001000100, 0b001001000, 0b001110000, 0b001001000},
  ['l'] = {0b000001000, 0b000001000, 0b000001000, 0b000001000, 0b000001000},
  ['m'] = {0b000000000, 0b001101100, 0b001010010, 0b001010010, 0b001000010},
  ['n'] = {0b000000000, 0b001111100, 0b001000010, 0b001000010, 0b001000010},
  ['o'] = {0b000000000, 0b000111100, 0b001000010, 0b001000010, 0b000111100},
  ['p'] = {0b000000000, 0b001111100, 0b001000010, 0b001111100, 0b001000000},
  ['q'] = {0b000000000, 0b000111110, 0b001000010, 0b000111110, 0b000000010},
  ['r'] = {0b000000000, 0b000111100, 0b001000000, 0b001000000, 0b001000000},
  ['s'] = {0b000000000, 0b000111110, 0b001000000, 0b000111100, 0b001111000},
  ['t'] = {0b000010000, 0b001111100, 0b000010000, 0b000010000, 0b000001000},
  ['u'] = {0b000000000, 0b001000010, 0b001000010, 0b001000010, 0b000111110},
  ['v'] = {0b000000000, 0b001000010, 0b001000010, 0b000100100, 0b000011000},
  ['w'] = {0b000000000, 0b001000010, 0b001010010, 0b001010010, 0b001101100},
  ['x'] = {0b000000000, 0b001000010, 0b000100100, 0b000011000, 0b001000010},
  ['y'] = {0b000000000, 0b001000010, 0b000111110, 0b000000010, 0b000111100},
  ['z'] = {0b000000000, 0b001111110, 0b000001100, 0b000110000, 0b001111110},

  // Digits
  ['0'] = {0b001111100, 0b010000010, 0b010000010, 0b010000010, 0b001111100},
  ['1'] = {0b000001000, 0b000011000, 0b000001000, 0b000001000, 0b000011100},
  ['2'] = {0b001111100, 0b000000010, 0b001111100, 0b010000000, 0b001111100},
  ['3'] = {0b001111100, 0b000000010, 0b000111100, 0b000000010, 0b001111100},
  ['4'] = {0b010000010, 0b010000010, 0b010000010, 0b011111110, 0b000000010},
  ['5'] = {0b001111110, 0b001000000, 0b001111100, 0b000000010, 0b001111100},
  ['6'] = {0b000111110, 0b001000000, 0b001111100, 0b001000010, 0b000111100},
  ['7'] = {0b001111110, 0b000000010, 0b000000100, 0b000001000, 0b000010000},
  ['8'] = {0b000111100, 0b001000010, 0b000111100, 0b001000010, 0b000111100},
  ['9'] = {0b000111100, 0b001000010, 0b000111110, 0b000000010, 0b000111100}};

// Draw a character at (x, y) from the Unicode codepoint
static void draw_char(uint32_t* data, int x, int y, uint32_t codepoint, uint32_t color,
                      int screen_width)
{
  // Default to a space or placeholder if the character is unsupported
  const uint8_t* glyph;

  // Check if the codepoint is within the supported range of your font
  if (codepoint < 128)
  {
    glyph = font[codepoint]; // For ASCII and supported characters
  }
  else
  {
    glyph = font['?']; // Fallback for unsupported characters
  }

  // Adjust glyph size if scaling from 5x7 to CHAR_WIDTH x CHAR_HEIGHT
  int glyph_width  = 5; // The original width of the font bitmap is 5 pixels
  int glyph_height = 7; // The original height of the font bitmap is 7 pixels

  // Loop through each row of the glyph (assumes 5-bit wide font data)
  for (int j = 0; j < glyph_height; j++)
  {
    uint8_t row = glyph[j]; // Get the bitmap row for the glyph

    // For each pixel in the row, draw it with scaling to match CHAR_WIDTH and CHAR_HEIGHT
    for (int i = 0; i < glyph_width; i++)
    {
      if (row & (1 << (glyph_width - 1 - i)))
      { // Check if the bit is set (indicating a pixel)

        // Scale the pixel size to match CHAR_WIDTH and CHAR_HEIGHT
        for (int scale_x = 0; scale_x < CHAR_WIDTH / glyph_width; scale_x++)
        {
          for (int scale_y = 0; scale_y < CHAR_HEIGHT / glyph_height; scale_y++)
          {
            int screen_x = x + (i * (CHAR_WIDTH / glyph_width)) + scale_x;
            int screen_y = y + (j * (CHAR_HEIGHT / glyph_height)) + scale_y;

            // Ensure we don't go out of bounds
            if (screen_x >= 0 && screen_x < screen_width && screen_y >= 0)
            {
              data[screen_y * screen_width + screen_x] = color; // Set the pixel color

              // Add extra pixels around for a bold/thicker effect
              if (screen_x + 1 < screen_width)
              {
                data[screen_y * screen_width + screen_x + 1] = color; // Right pixel
              }
              if (screen_y + 1 < screen_width)
              {
                data[(screen_y + 1) * screen_width + screen_x] = color; // Bottom pixel
              }
              if (screen_x + 1 < screen_width && screen_y + 1 < screen_width)
              {
                data[(screen_y + 1) * screen_width + screen_x + 1] = color; // Bottom-right pixel
              }
            }
          }
        }
      }
    }
  }
}

#endif // UNICODE_H
