#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/GlobalFuncs.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/config/ConfigHandler.hpp>
#include <anvilock/include/freetype/FreeTypeHandler.hpp>
#include <anvilock/include/shaders/ShaderHandler.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <anvilock/external/stb/stb_image.h>

namespace anvlk::render
{

class EGLRenderer
{
public:
  explicit EGLRenderer(ClientState& state);

  void initEGL();
  void updateTimeTexture();
  void renderTimeBox();
  void renderPasswordField();

  auto createTextureShaderProgram(const types::Directory&    shaderRuntimeDir,
                                  anvlk::gfx::ShaderManager& shaderManager) -> GLuint;
  auto createTextTexture(const std::string& text) -> GLuint;
  auto loadTexture(const types::Path& filepath) -> GLuint;

private:
  types::wayland::WLDisplay_* m_wlDisplay;
  types::wayland::WLSurface_* m_wlSurface;

  EGLDisplay                         m_eglDisplay = EGL_NO_DISPLAY;
  EGLContext                         m_eglContext = EGL_NO_CONTEXT;
  EGLSurface                         m_eglSurface = EGL_NO_SURFACE;
  types::wayland::egl::WLEglWindow_* m_eglWindow  = nullptr;

  GLuint timeTexture_ = 0;

  logger::LogContext& m_logCtx;
  FT_Library          m_ftLibrary;
  FT_Face             m_ftFace;
};

} // namespace anvlk::render
