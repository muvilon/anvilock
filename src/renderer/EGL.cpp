#include <anvilock/include/Types.hpp>
#include <anvilock/include/renderer/EGL.hpp>
#include <anvilock/include/renderer/GLUtils.hpp>
#include <anvilock/include/utils/Assert.hpp>
#include <anvilock/include/widgets/BackgroundTexture.hpp>
#include <anvilock/include/widgets/PasswordField.hpp>
#include <anvilock/include/widgets/TimeBox.hpp>

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
    cs.shaderState.bgTexture = widgets::loadBGTexture(cs);
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

  widgets::timebox::updateTimeTexture(cs);
  widgets::renderTimeBox(cs);
  widgets::renderPasswordField(cs);

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
