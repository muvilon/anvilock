#include <anvilock/include/Types.hpp>
#include <anvilock/include/freetype/FreeTypeStruct.hpp>
#include <anvilock/include/renderer/EGL.hpp>
#include <anvilock/include/renderer/GLUtils.hpp>
#include <anvilock/include/utils/STBImpl.hpp>

namespace anvlk::render
{

static auto initEGLConfig(ClientState& cs) -> EGLConfig
{
  cs.eglDisplay = eglGetDisplay((EGLNativeDisplayType)cs.wlDisplay);

  if (cs.eglDisplay == EGL_NO_DISPLAY)
  {
    LOG::ERROR(cs.logCtx, "Failed to get EGL Display!");
    throw EGLErrorException("!!! FAILED TO GET EGL DISPLAY !!!");
  }

  if (!eglInitialize(cs.eglDisplay, nullptr, nullptr))
  {
    LOG::ERROR(cs.logCtx, "Failed to initialize EGL!");
    throw EGLErrorException("!!! FAILED TO INIT EGL !!!");
  }

  if (!eglBindAPI(EGL_OPENGL_ES_API))
  {
    LOG::ERROR(cs.logCtx, "Failed to bind OpenGL ES API!");
    throw EGLErrorException("!!! FAILED TO BIND OPENGL ES API !!!");
  }

  EGLConfig eglConfig;
  EGLint    numConfigs;

  if (!eglChooseConfig(cs.eglDisplay, GLOBAL_EGL_ATTRIBS.data(), &eglConfig, 1, &numConfigs) ||
      numConfigs < 1)
  {
    LOG::ERROR(cs.logCtx, "Failed to choose EGL config!");
    throw EGLErrorException("!!! FAILED TO CHOOSE EGL CONFIG !!!");
  }

  return eglConfig;
}

static auto createTextTexture(ClientState& state, const std::string& text) -> GLuint
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
  types::uint x_offset = 0;
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
  if (texture == 0)
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

static auto loadBGTexture(ClientState& cs) -> GLuint
{
  types::Dimensions width, height;
  int               channels;

  LOG::DEBUG(cs.logCtx, "Loading BG of name: '{}'", cs.userConfig.bg.name);

  types::PathCStr filePath = cs.userConfig.bg.path.c_str();

  types::ImageData* image = stbi_load(filePath, &width, &height, &channels, STBI_rgb_alpha);

  if (!image)
  {
    LOG::ERROR(cs.logCtx, "Failed to load image: {}", filePath);
    throw EGLErrorException(
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

static void updateTimeTexture(ClientState& cs)
{
  // TODO: Update the texture containing the current time
  types::TimeString timeStr = utils::getTimeString(cs.userConfig.time.time_format);

  static types::TimeString lastRecordedTimeStr = "";

  if (lastRecordedTimeStr != timeStr)
  {
    LOG::TRACE(cs.logCtx, "Time changed from '{}' to '{}'", lastRecordedTimeStr, timeStr);
    lastRecordedTimeStr = timeStr;

    if (cs.timeTexture)
    {
      glDeleteTextures(1, &cs.timeTexture);
      cs.timeTexture = 0;
      LOG::TRACE(cs.logCtx, "Old time texture deleted.");
    }

    cs.timeTexture = createTextTexture(cs, timeStr);
  }
}

static void renderTimeBox(ClientState& cs)
{
  if (!cs.timeTexture)
  {
    LOG::ERROR(cs.logCtx, "No valid texture for rendering.");
    return;
  }

  static GLuint VBO = 0;

  if (VBO == 0)
  {
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cs.userConfig.timeBoxVertices),
                 cs.userConfig.timeBoxVertices.data(), GL_STATIC_DRAW);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        (void*)(2 * sizeof(GLfloat)));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, cs.timeTexture);

  glUseProgram(cs.shaderState.textureShaderProgram);
  glUniform1i(glGetUniformLocation(cs.shaderState.textureShaderProgram, "uTexture"), 0);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    LOG::ERROR(cs.logCtx, "OpenGL error: {}", error);
  }

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_BLEND);

  LOG::TRACE(cs.logCtx, "Time box rendered successfully!!");
}

static void renderPasswordField(ClientState& state)
{
  GLuint program =
    render::GLUtils::createShaderProgram<anvlk::gfx::ShaderID::RENDER_PWD_FIELD_EGL_VERTEX,
                                         anvlk::gfx::ShaderID::RENDER_PWD_FIELD_EGL_FRAG>(
      state.logCtx, *state.shaderManagerPtr);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Use the shader program
  glUseProgram(program);

  // Get uniform locations
  GLint color_location    = glGetUniformLocation(program, "color");
  GLint offset_location   = glGetUniformLocation(program, "offset");
  GLint position_location = glGetAttribLocation(program, "position");

  // Width and height of the password field
  float field_width  = 0.7f;  // Adjusted width for the field
  float field_height = 0.15f; // Adjusted height for the field

  // Position offset to center at the bottom of the screen
  float offset_x = 0;                           // Horizontally center the field
  float offset_y = -0.8f + field_height / 2.0f; // Vertically align it at the bottom

  // Set up the password field background (using GL_TRIANGLE_STRIP for a rectangle)
  glUniform4f(color_location, 1.0f, 1.0f, 1.0f, 0.70f); // Light background with transparency
  glUniform2f(offset_location, offset_x, offset_y);

  glVertexAttribPointer(GLUtils::to_gluint(position_location), 2, GL_FLOAT, GL_FALSE, 0,
                        utils::password_field_vertices.data());
  glEnableVertexAttribArray(GLUtils::to_gluint(position_location));

  // Draw the background of the password field
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Draw the border with a subtle shadow effect
  glUniform4f(color_location, 0.8f, 0.8f, 0.8f, 1.0f);
  glDrawArrays(GL_LINE_LOOP, 0, 4);

  // Draw password dots
  glUniform4f(color_location, 0.3f, 0.3f, 0.3f, 0.8f); // Gray dots

  // Set up vertices for dots
  glVertexAttribPointer(GLUtils::to_gluint(position_location), 2, GL_FLOAT, GL_FALSE, 0,
                        utils::dot_vertices.data());

  // Adjust dot positions based on password input
  float dot_spacing = field_width / types::to_float(state.pamState.passwordIndex + 1);
  for (types::iters i = 0; i < state.pamState.passwordIndex; i++)
  {
    float x_position =
      offset_x + types::to_float(i + 1) * dot_spacing - field_width / 2; // Center the dots
    glUniform2f(offset_location, x_position, offset_y);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  // Handle Authentication Failure (Red border for failure)
  if (state.pamState.authState.authFailed)
  {
    types::FloatArray<4> failColor = {1.0f, 0.0f, 0.0f, 1.0f}; // Red for failure

    glUniform4fv(color_location, 1, failColor.data());
    glUniform2f(offset_location, offset_x, offset_y);
    glDrawArrays(GL_LINE_LOOP, 0, 4); // Re-draw border with failure color
  }

  // Handle Authentication Success (Green border for success)
  if (!state.pamState.authState.authFailed && state.pamState.passwordIndex > 0)
  {
    types::FloatArray<4> successColor = {0.0f, 1.0f, 0.0f, 1.0f}; // Green for success

    glUniform4fv(color_location, 1, successColor.data());
    glUniform2f(offset_location, offset_x, offset_y);
    glDrawArrays(GL_LINE_LOOP, 0, 4); // Re-draw border with success color
  }

  // Disable blending and clean up
  glDisable(GL_BLEND);
  glDeleteProgram(program);
}

void renderLockScreen(ClientState& cs)
{
  logger::switchCtx(cs.logCtx, logger::LogCategory::EGL);
  if (!eglMakeCurrent(cs.eglDisplay, cs.eglSurface, cs.eglSurface, cs.eglContext))
  {
    LOG::ERROR(cs.logCtx, "Failed to make EGL the current context!");
    throw EGLErrorException("!!! FAILED TO MAKE EGL CURRENT CTX !!!");
  }

  static bool initialized = false;

  if (!initialized)
  {
    cs.shaderState.bgTexture = loadBGTexture(cs);
    cs.shaderState.textureShaderProgram =
      render::GLUtils::createShaderProgram<anvlk::gfx::ShaderID::TEXTURE_EGL_VERTEX,
                                           anvlk::gfx::ShaderID::TEXTURE_EGL_FRAG>(
        cs.logCtx, *cs.shaderManagerPtr);
    initialized = true;
  }

  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(cs.shaderState.textureShaderProgram);

  GLint posLoc      = glGetAttribLocation(cs.shaderState.textureShaderProgram, "position");
  GLint texCoordLoc = glGetAttribLocation(cs.shaderState.textureShaderProgram, "texCoord");

  glVertexAttribPointer(GLUtils::to_gluint(posLoc), 2, GL_FLOAT, GL_FALSE, 0,
                        utils::quad_vertices.data());
  glEnableVertexAttribArray(GLUtils::to_gluint(posLoc));

  glVertexAttribPointer(GLUtils::to_gluint(texCoordLoc), 2, GL_FLOAT, GL_FALSE, 0,
                        utils::tex_coords.data());
  glEnableVertexAttribArray(GLUtils::to_gluint(texCoordLoc));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, cs.shaderState.bgTexture);
  glUniform1i(glGetUniformLocation(cs.shaderState.textureShaderProgram, "uTexture"), 0);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  updateTimeTexture(cs);
  renderTimeBox(cs);
  renderPasswordField(cs);

  eglSwapBuffers(cs.eglDisplay, cs.eglSurface);

  logger::resetCtx(cs.logCtx);
}

void initEGL(ClientState& cs)
{
  cs.eglConfig = initEGLConfig(cs);

  cs.eglContext =
    eglCreateContext(cs.eglDisplay, cs.eglConfig, EGL_NO_CONTEXT, EGL_CTX_ATTRIBS.data());

  if (cs.eglConfig == EGL_NO_CONTEXT)
  {
    LOG::ERROR(cs.logCtx, "Failed to create EGL context!");
    throw EGLErrorException("!!! FAILED TO CREATE EGL CTX !!!");
  }

  wl_display_roundtrip(cs.wlDisplay);

  types::Dimensions width =
    cs.outputState.width > 0 ? cs.outputState.width : utils::FallbackScreenWidth;

  types::Dimensions height =
    cs.outputState.height > 0 ? cs.outputState.height : utils::FallbackScreenHeight;

  cs.eglWindow = wl_egl_window_create(cs.wlSurface, width, height);

  if (!cs.eglWindow)
  {
    LOG::ERROR(cs.logCtx, "Failed to create EGL window!");
    throw EGLErrorException("!!! FAILED TO CREATE EGL WINDOW !!!");
  }

  cs.eglSurface =
    eglCreateWindowSurface(cs.eglDisplay, cs.eglConfig, (EGLNativeWindowType)cs.eglWindow, nullptr);

  if (cs.eglSurface == EGL_NO_SURFACE)
  {
    LOG::ERROR(cs.logCtx, "Failed to create EGL surface!");
    throw EGLErrorException("!!! FAILED TO CREATE EGL SURFACE !!!");
  }

  glViewport(0, 0, width, height);

  renderLockScreen(cs);
}

} // namespace anvlk::render
