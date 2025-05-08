#include <EGL/egl.h>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/renderer/EGL.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <anvilock/external/stb/stb_image.h>

namespace anvlk::render
{

auto initEGLConfig(ClientState& cs) -> EGLConfig
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
    if (FT_Load_Char(state.freeTypeState.ftFace, c, FT_LOAD_RENDER))
    {
      LOG::ERROR(state.logCtx, "Failed to load glyph.");
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
    LOG::ERROR(state.logCtx, "Failed to allocate image buffer.");
    return types::EGLCodes::RET_CODE_FAIL;
  }

  // Second pass: render glyphs
  int x_offset = 0;
  for (const char& c : text)
  {
    if (FT_Load_Char(state.freeTypeState.ftFace, c, FT_LOAD_RENDER))
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

auto loadBGTexture(ClientState& cs) -> GLuint
{
  types::Dimensions width, height;
  int               channels;

  LOG::DEBUG(cs.logCtx, "Loading BG of name: {}", cs.userConfig.bg.name);

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

  return 0;
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

  if (!eglMakeCurrent(cs.eglDisplay, cs.eglSurface, cs.eglSurface, cs.eglContext))
  {
    LOG::ERROR(cs.logCtx, "Failed to make EGL the current context!");
    throw EGLErrorException("!!! FAILED TO MAKE EGL CURRENT CTX !!!");
  }

  glViewport(0, 0, width, height);

  GLuint bgTexture = loadBGTexture(cs);

  glBindTexture(GL_TEXTURE_2D, bgTexture);

  glClear(GL_COLOR_BUFFER_BIT);

  const auto eglVertexOpt =
    cs.shaderManagerPtr->getShaderSource(anvlk::gfx::ShaderID::INIT_EGL_VERTEX);
  const auto eglFragOpt = cs.shaderManagerPtr->getShaderSource(anvlk::gfx::ShaderID::INIT_EGL_FRAG);

  if (eglVertexOpt && eglFragOpt)
  {
    types::ShaderContentCStr eglVertexSrc = cs.shaderManagerPtr->asCString(eglVertexOpt);
    types::ShaderContentCStr eglFragSrc   = cs.shaderManagerPtr->asCString(eglFragOpt);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader   = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &eglVertexSrc, nullptr);
    glShaderSource(fragShader, 1, &eglFragSrc, nullptr);
    glCompileShader(vertexShader);
    glCompileShader(fragShader);

    GLint compileStatus;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_FALSE)
    {
      LOG::ERROR(cs.logCtx, "Vertex shader compilation failed!");
      throw EGLErrorException("!!! FAILED TO COMPILE EGL VERTEX SHADER !!!");
    }

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_FALSE)
    {
      LOG::ERROR(cs.logCtx, "Fragment shader compilation failed!");
      throw EGLErrorException("!!! FAILED TO COMPILE EGL FRAG SHADER !!!");
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    GLint linkStatus;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
      LOG::ERROR(cs.logCtx, "Shader program linking failed!");
      throw EGLErrorException("!!! FAILED TO LINK SHADER PROGRAM !!!");
    }

    glUseProgram(shaderProgram);

    GLint posLoc      = glGetAttribLocation(shaderProgram, "position");
    GLint texCoordLoc = glGetAttribLocation(shaderProgram, "texCoord");

    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, utils::quad_vertices.data());
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, utils::tex_coords.data());
    glEnableVertexAttribArray(texCoordLoc);

    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    updateTimeTexture(cs);
    renderTimeBox(cs);
    //renderPasswordField(cs);

    eglSwapBuffers(cs.eglDisplay, cs.eglSurface);
  }
  else
  {
    LOG::ERROR(cs.logCtx, "Failed to validate shader source!");
  }
}

void renderPasswordField(ClientState& state)
{
  const auto pwdFieldVertexOpt =
    state.shaderManagerPtr->getShaderSource(anvlk::gfx::ShaderID::RENDER_PWD_FIELD_EGL_VERTEX);
  const auto pwdFieldFragOpt =
    state.shaderManagerPtr->getShaderSource(anvlk::gfx::ShaderID::RENDER_PWD_FIELD_EGL_FRAG);

  if (pwdFieldVertexOpt && pwdFieldFragOpt)
  {
    types::ShaderContentCStr pwdFieldVertexSrc =
      state.shaderManagerPtr->asCString(pwdFieldVertexOpt);
    types::ShaderContentCStr pwdFieldFragSrc = state.shaderManagerPtr->asCString(pwdFieldFragOpt);

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &pwdFieldVertexSrc, nullptr);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &pwdFieldFragSrc, nullptr);
    glCompileShader(fragment_shader);

    // Create and link program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // Use the shader program
    glUseProgram(program);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth testing for 2D UI elements (password field)
    glDisable(GL_DEPTH_TEST);

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

    glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0,
                          utils::password_field_vertices.data());
    glEnableVertexAttribArray(position_location);

    // Draw the background of the password field
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Draw the border with a subtle shadow effect
    glUniform4f(color_location, 0.8f, 0.8f, 0.8f, 1.0f);
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    // Draw password dots
    glUniform4f(color_location, 0.3f, 0.3f, 0.3f, 0.8f); // Gray dots

    // Set up vertices for dots
    glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, utils::dot_vertices.data());

    // Adjust dot positions based on password input
    float dot_spacing = field_width / (state.pam.passwordIndex + 1);
    for (int i = 0; i < state.pam.passwordIndex; i++)
    {
      float x_position = offset_x + (i + 1) * dot_spacing - field_width / 2; // Center the dots
      glUniform2f(offset_location, x_position, offset_y);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // Handle Authentication Failure (Red border for failure)
    if (state.pam.authState.authFailed)
    {
      types::FloatArray<4> failColor = {1.0f, 0.0f, 0.0f, 1.0f}; // Red for failure

      glUniform4fv(color_location, 1, failColor.data());
      glUniform2f(offset_location, offset_x, offset_y);
      glDrawArrays(GL_LINE_LOOP, 0, 4); // Re-draw border with failure color
    }

    // Handle Authentication Success (Green border for success)
    if (!state.pam.authState.authFailed && state.pam.passwordIndex > 0)
    {
      types::FloatArray<4> successColor = {0.0f, 1.0f, 0.0f, 1.0f}; // Green for success

      glUniform4fv(color_location, 1, successColor.data());
      glUniform2f(offset_location, offset_x, offset_y);
      glDrawArrays(GL_LINE_LOOP, 0, 4); // Re-draw border with success color
    }

    // Disable blending and clean up
    glDisable(GL_BLEND);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(program);

    // Swap buffers to render the final frame
    eglSwapBuffers(state.eglDisplay, state.eglSurface);

    if (state.pam.authState.authFailed)
      sleep(1);
  }
  else
  {
    LOG::ERROR(state.logCtx, "Failed to allocate memory, causing pwd render to fail!");
  }
}

void updateTimeTexture(ClientState& cs)
{
  // TODO: Update the texture containing the current time
  types::TimeString timeStr = utils::getTimeString(cs.userConfig.time.time_format);

  static types::TimeString lastRecordedTimeStr = "";

  if (lastRecordedTimeStr != timeStr)
  {
    LOG::DEBUG(cs.logCtx, "Time changed from '{}' to '{}'", lastRecordedTimeStr, timeStr);
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

void renderTimeBox(ClientState& cs)
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

auto createTextureShaderProgram(anvlk::gfx::ShaderManager& shaderManager) -> GLuint
{
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

  return types::EGLCodes::RET_CODE_FAIL;
}

void renderLockScreen(ClientState& cs)
{
  logger::switchCtx(cs.logCtx, logger::LogCategory::EGL);
  if (!eglMakeCurrent(cs.eglDisplay, cs.eglSurface, cs.eglSurface, cs.eglContext))
  {
    LOG::ERROR(cs.logCtx, "Failed to make EGL the current context!");
    throw EGLErrorException("!!! FAILED TO MAKE EGL CURRENT CTX !!!");
  }

  static GLuint bgTexture            = 0;
  static GLuint textureShaderProgram = 0;
  static int    initialized          = 0;

  if (!initialized)
  {
    LOG::WARN(cs.logCtx, "EGL not initialized!");
    bgTexture            = loadBGTexture(cs);
    textureShaderProgram = createTextureShaderProgram(*cs.shaderManagerPtr);
    initialized          = 1;
  }

  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(textureShaderProgram);

  GLint posLoc      = glGetAttribLocation(textureShaderProgram, "position");
  GLint texCoordLoc = glGetAttribLocation(textureShaderProgram, "texCoord");

  glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, utils::quad_vertices.data());
  glEnableVertexAttribArray(posLoc);

  glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, utils::tex_coords.data());
  glEnableVertexAttribArray(texCoordLoc);

  // Bind and render texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, bgTexture);
  glUniform1i(glGetUniformLocation(textureShaderProgram, "uTexture"), 0);

  updateTimeTexture(cs);
  renderTimeBox(cs);
  //renderPasswordField(cs);
  logger::resetCtx(cs.logCtx);
}

} // namespace anvlk::render
