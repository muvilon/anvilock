#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/GlobalFuncs.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/freetype/FreeTypeHandler.hpp>
#include <anvilock/include/renderer/EGL.hpp>
#include <anvilock/include/runtime/EGLExceptionHandler.hpp>
#include <anvilock/include/shaders/ShaderHandler.hpp>

namespace anvlk::render
{

inline constexpr std::array<EGLint, 11> GLOBAL_EGL_ATTRIBS = {EGL_RENDERABLE_TYPE,
                                                              EGL_OPENGL_ES2_BIT,
                                                              EGL_SURFACE_TYPE,
                                                              EGL_WINDOW_BIT,
                                                              EGL_RED_SIZE,
                                                              8,
                                                              EGL_GREEN_SIZE,
                                                              8,
                                                              EGL_BLUE_SIZE,
                                                              8,
                                                              EGL_NONE};

inline constexpr std::array<EGLint, 3> EGL_CTX_ATTRIBS = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

auto initEGLConfig(ClientState& cs) -> EGLConfig;
auto createTextTexture(ClientState& state, const std::string& text) -> GLuint;
auto loadTexture(ClientState& cs) -> GLuint;
void initEGL(ClientState& cs);
void updateTimeTexture(ClientState& cs);
void renderTimeBox(ClientState& cs);
auto createTextureShaderProgram(anvlk::gfx::ShaderManager& shaderManager) -> GLuint;
void renderLockScreen(ClientState& cs);

} // namespace anvlk::render
