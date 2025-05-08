#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/GlobalFuncs.hpp>
#include <anvilock/include/LogMacros.hpp>
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

void initEGL(ClientState& cs);
void renderLockScreen(ClientState& cs);

} // namespace anvlk::render
