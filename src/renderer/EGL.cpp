#include <anvilock/include/renderer/EGL.hpp>

namespace anvlk::render
{

EGLRenderer::EGLRenderer(ClientState& state)
    : m_wlDisplay(state.wlDisplay), m_wlSurface(state.wlSurface), m_eglWindow(state.eglWindow),
      m_logCtx(state.logCtx), m_ftLibrary(state.freeTypeState.ftLibrary),
      m_ftFace(state.freeTypeState.ftFace)
{
  // Constructor stub
}

void EGLRenderer::initEGL()
{
  // TODO: Initialize EGL display, context, surface, etc.
}

void EGLRenderer::updateTimeTexture()
{
  // TODO: Update the texture containing the current time
}

void EGLRenderer::renderTimeBox()
{
  // TODO: Render a box displaying the current time
}

void EGLRenderer::renderPasswordField()
{
  // TODO: Render the password input field
}

auto EGLRenderer::createTextureShaderProgram(const types::Directory&    shaderRuntimeDir,
                                             anvlk::gfx::ShaderManager& shaderManager) -> GLuint
{
  // TODO: Create and return a texture shader program from shader directory

  const auto textureVertexOpt =
    shaderManager.getShaderSource(anvlk::gfx::ShaderID::TEXTURE_EGL_VERTEX);
  const auto textureFragOpt = shaderManager.getShaderSource(anvlk::gfx::ShaderID::TEXTURE_EGL_FRAG);

  if (textureFragOpt && textureVertexOpt)
  {
    types::ShaderContentCStr textureVertexSrc = shaderManager.asCString(textureVertexOpt);
    types::ShaderContentCStr textureFragSrc   = shaderManager.asCString(textureFragOpt);

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &textureVertexSrc, nullptr);
    glCompileShader(vertex_shader);

    // Create and compile fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &textureFragSrc, nullptr);
    glCompileShader(fragment_shader);

    // Create and link shader program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // Clean up shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
  }
  else
  {
    //
  }

  return types::EGLCodes::RET_CODE_FAIL;
}

auto EGLRenderer::createTextTexture(const std::string& text) -> GLuint
{
  // TODO: Render text using FreeType and return a GL texture

  if (text.empty())
  {
    logger::log(logger::LogLevel::Error, m_logCtx, "Invalid text string for texture.");
    return types::EGLCodes::RET_CODE_FAIL;
  }

  FT_GlyphSlot slot = m_ftFace->glyph;

  types::Dimensions width       = 0;
  types::Dimensions maxHeight   = 0;
  i64               maxBearingY = 0;

  for (const char& c : text)
  {
    if (FT_Load_Char(m_ftFace, c, FT_LOAD_RENDER))
    {
      logger::log(logger::LogLevel::Error, m_logCtx, "Failed to load glyph.");
      continue;
    }

    // Add proper spacing between characters
    width += slot->bitmap.width + 1; // +1 for spacing
    maxHeight   = types::u32(maxHeight) > slot->bitmap.rows ? maxHeight : slot->bitmap.rows;
    maxBearingY = FT_Int(maxBearingY) > slot->bitmap_top ? maxBearingY : slot->bitmap_top;
  }

  types::Dimensions height =
    freetype::CHAR_HEIGHT_PX > maxHeight ? freetype::CHAR_HEIGHT_PX : maxHeight;

  width  = anvlk::utils::nextPowerOfTwo(width);
  height = anvlk::utils::nextPowerOfTwo(height);

  auto image = std::make_unique<types::ImageData[]>(width * height);

  if (!image)
  {
    logger::log(logger::LogLevel::Error, m_logCtx, "Failed to allocate image buffer.");
    return types::EGLCodes::RET_CODE_FAIL;
  }

  // Second pass: render glyphs
  int x_offset = 0;
  for (const char& c : text)
  {
    if (FT_Load_Char(m_ftFace, c, FT_LOAD_RENDER))
      continue;

    // Calculate vertical position for baseline alignment
    int y_offset = (maxBearingY - slot->bitmap_top) + (height - maxHeight) / 2;

    for (unsigned int row = 0; row < slot->bitmap.rows; row++)
    {
      for (unsigned int col = 0; col < slot->bitmap.width; col++)
      {
        int dst_row = row + y_offset;
        if (dst_row >= 0 && dst_row < height)
        {
          image[dst_row * width + x_offset + col] =
            slot->bitmap.buffer[row * slot->bitmap.width + col];
        }
      }
    }

    x_offset += slot->bitmap.width + 1; // +1 for spacing
  }

  // Create OpenGL texture
  GLuint texture;
  glGenTextures(1, &texture);
  if (texture == 0)
  {
    logger::log(logger::LogLevel::Error, m_logCtx, "Failed to generate OpenGL ES texture.");
    return types::EGLCodes::RET_CODE_FAIL;
  }

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE,
               image.get());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  logger::log(logger::LogLevel::Debug, m_logCtx, "Text texture successfully created!");
  return texture;
}

auto EGLRenderer::loadTexture(const types::Path& filepath) -> GLuint
{
  // TODO: Load image file with stb_image and upload it to OpenGL as a texture
  return 0;
}

} // namespace anvlk::render
