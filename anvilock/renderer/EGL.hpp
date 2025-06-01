#ifndef ANVLK_RENDERER_EGL_HPP
#define ANVLK_RENDERER_EGL_HPP

#include <anvilock/ClientState.hpp>
#include <anvilock/GlobalFuncs.hpp>
#include <anvilock/LogMacros.hpp>
#include <anvilock/Types.hpp>
#include <anvilock/freetype/FreeTypeHandler.hpp>
#include <anvilock/runtime/EGLExceptionHandler.hpp>
#include <anvilock/shaders/ShaderHandler.hpp>

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

void initEGL(ClientState& cs);
void renderLockScreen(ClientState& cs);

} // namespace anvlk::render

#endif
