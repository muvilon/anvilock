#include "anvilock/Types.hpp"
#include <anvilock/renderer/GLUtils.hpp>
#include <anvilock/widgets/helpers/WidgetHelper.hpp>

namespace anvlk::widgets::helpers
{

auto createTextTexture(ClientState& state, const std::string& text) -> GLuint
{
  if (text.empty())
  {
    LOG::ERROR(state.logCtx, "Empty string passed to createTextTexture.");
    return 0;
  }

  FT_Face      face = state.freeTypeState.ftFace;
  FT_GlyphSlot slot = face->glyph;

  types::Dimensions width       = 0;
  types::Dimensions maxHeight   = 0;
  types::Dimensions maxBearingY = 0;

  for (char c : text)
  {
    if (FT_Load_Char(face, freetype::to_ft_ulong(c), FT_LOAD_RENDER))
      continue;
    width += types::Dimensions(slot->bitmap.width + 1);
    maxHeight =
      types::u32(maxHeight) > slot->bitmap.rows ? maxHeight : types::Dimensions(slot->bitmap.rows);
    maxBearingY = FT_Int(maxBearingY) > slot->bitmap_top ? maxBearingY : slot->bitmap_top;
  }

  types::Dimensions height = std::max<types::Dimensions>(freetype::CHAR_HEIGHT_PX, maxHeight);

  // Add extra space for blur/shadow offset
  const int SHADOW_OFFSET = 3;
  const int BLUR_MARGIN   = 4;
  width                   = utils::nextPowerOfTwo(width + SHADOW_OFFSET + BLUR_MARGIN);
  height                  = utils::nextPowerOfTwo(height + SHADOW_OFFSET + BLUR_MARGIN);

  struct RGBA
  {
    u8 r, g, b, a;
  };
  auto buffer = std::make_unique<RGBA[]>(types::to_usize(width * height));
  std::fill(buffer.get(), buffer.get() + (width * height), RGBA{.r = 0, .g = 0, .b = 0, .a = 0});

  uint x_offset = SHADOW_OFFSET;
  for (char c : text)
  {
    if (FT_Load_Char(face, freetype::to_ft_ulong(c), FT_LOAD_RENDER))
      continue;

    const auto& bmp      = slot->bitmap;
    i64         y_offset = (maxBearingY - slot->bitmap_top) + (height - maxHeight) / 2;

    for (uint row = 0; row < bmp.rows; ++row)
    {
      for (uint col = 0; col < bmp.width; ++col)
      {
        u8 value = bmp.buffer[row * types::to_usize(bmp.pitch) + col];

        int dst_row = types::to_int(row + y_offset);
        int dst_col = types::to_int(x_offset + col);

        if (dst_row >= 0 && dst_row < static_cast<int>(height) && dst_col >= 0 &&
            dst_col < static_cast<int>(width))
        {
          types::iters idx = types::to_usize(dst_row * width + dst_col);

          // Write shadow first: darker, slightly offset
          int shadow_row = dst_row + SHADOW_OFFSET;
          int shadow_col = dst_col + SHADOW_OFFSET;
          if (shadow_row < height && shadow_col < width)
          {
            types::iters shadow_idx = types::to_usize(shadow_row * width + shadow_col);
            auto&        s          = buffer[shadow_idx];
            s.r = s.g = s.b = 0;
            s.a             = std::max<u8>(s.a, value / 2); // softer alpha for shadow
          }

          // Write main glyph: bright white, full opacity
          auto& px = buffer[idx];
          px.r = 48, px.g = 152, px.b = 196;
          px.a = std::max<u8>(px.a, value);
        }
      }
    }

    x_offset += bmp.width + 1;
  }

  // Create GL texture
  GLuint texture = 0;
  glGenTextures(1, &texture);
  if (texture == 0)
  {
    LOG::ERROR(state.logCtx, "Failed to generate GL texture.");
    return 0;
  }

  glBindTexture(GL_TEXTURE_2D, texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               buffer.get());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  LOG::TRACE(state.logCtx, "Created text texture for '{}': {}x{}", text, width, height);
  return texture;
}

} // namespace anvlk::widgets::helpers
