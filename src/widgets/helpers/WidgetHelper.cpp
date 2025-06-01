#include <anvilock/renderer/GLUtils.hpp>
#include <anvilock/widgets/helpers/WidgetHelper.hpp>

namespace anvlk::widgets::helpers
{

auto createTextTexture(ClientState& state, const std::string& text) -> GLuint
{
  if (text.empty())
  {
    LOG::ERROR(state.logCtx, "Invalid text string for texture.");
    return types::EGLCodes::RET_CODE_FAIL;
  }

  FT_GlyphSlot slot = state.freeTypeState.ftFace->glyph;

  types::Dimensions width       = 0;
  types::Dimensions maxHeight   = 0;
  i64               maxBearingY = 0;

  for (const char& c : text)
  {
    if (FT_Load_Char(state.freeTypeState.ftFace, freetype::to_ft_ulong(c), FT_LOAD_RENDER))
    {
      LOG::ERROR(state.logCtx, "Failed to load glyph.");
      continue;
    }

    // Add proper spacing between characters
    width += types::Dimensions(slot->bitmap.width + 1); // +1 for spacing
    maxHeight =
      types::u32(maxHeight) > slot->bitmap.rows ? maxHeight : types::Dimensions(slot->bitmap.rows);
    maxBearingY = FT_Int(maxBearingY) > slot->bitmap_top ? maxBearingY : slot->bitmap_top;
  }

  types::Dimensions height =
    freetype::CHAR_HEIGHT_PX > maxHeight ? freetype::CHAR_HEIGHT_PX : maxHeight;

  width  = utils::nextPowerOfTwo(width);
  height = utils::nextPowerOfTwo(height);

  auto image = std::make_unique<types::ImageData[]>(types::to_usize(width * height));
  std::fill(image.get(), image.get() + (width * height), 0);

  if (!image)
  {
    LOG::ERROR(state.logCtx, "Failed to allocate image buffer.");
    return types::EGLCodes::RET_CODE_FAIL;
  }

  // Second pass: render glyphs
  uint x_offset = 0;
  for (const char& c : text)
  {
    if (FT_Load_Char(state.freeTypeState.ftFace, freetype::to_ft_ulong(c), FT_LOAD_RENDER))
      continue;

    // Calculate vertical position for baseline alignment
    i64 y_offset = (maxBearingY - slot->bitmap_top) + (height - maxHeight) / 2;

    for (uint row = 0; row < slot->bitmap.rows; row++)
    {
      for (uint col = 0; col < slot->bitmap.width; col++)
      {
        i64 dst_row = row + y_offset;
        if (dst_row >= 0 && dst_row < height)
        {
          image[types::to_usize(dst_row * width + x_offset + col)] =
            slot->bitmap.buffer[row * slot->bitmap.width + col];
        }
      }
    }

    x_offset += slot->bitmap.width + 1; // +1 for spacing
  }

  // Create OpenGL texture
  GLuint texture;
  glGenTextures(1, &texture);
  if (texture == render::GLUtils::TextureStatus::TEXTURE_DESTROY)
  {
    LOG::ERROR(state.logCtx, "Failed to generate OpenGL ES texture.");
    return types::EGLCodes::RET_CODE_FAIL;
  }

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE,
               image.get());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  LOG::TRACE(state.logCtx, "Text texture successfully created!");

  return texture;
}

} // namespace anvlk::widgets::helpers
