#include <anvilock/include/runtime/EGLExceptionHandler.hpp>
#include <anvilock/include/utils/STBImpl.hpp>
#include <anvilock/include/widgets/BackgroundTexture.hpp>

namespace anvlk::widgets
{

auto loadBGTexture(ClientState& cs) -> GLuint
{
  types::Dimensions width, height;
  int               channels;

  LOG::DEBUG(cs.logCtx, "Loading BG of name: '{}'", cs.userConfig.bg.name);

  types::PathCStr filePath = cs.userConfig.bg.path.c_str();

  types::ImageData* image = stbi_load(filePath, &width, &height, &channels, STBI_rgb_alpha);

  if (!image)
  {
    LOG::ERROR(cs.logCtx, "Failed to load image: {}", filePath);
    throw render::EGLErrorException(
      "!!! This should be replaced with fallback file path texture loading !!!");
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(image);
  return texture;
}

} // namespace anvlk::widgets
