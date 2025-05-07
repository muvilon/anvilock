#pragma once

#include <EGL/egl.h>
#include <anvilock/include/Types.hpp>
#include <format>
#include <stdexcept>

namespace anvlk::render
{

class EGLErrorException : public std::runtime_error
{
public:
  EGLErrorException(const anvlk::types::RuntimeErrorMessage& message,
                    EGLint                                   errorCode = eglGetError())
      : std::runtime_error(composeMessage(message, errorCode)), m_errorCode(errorCode)
  {
  }

  [[nodiscard]] auto errorCode() const noexcept -> EGLint { return m_errorCode; }

private:
  EGLint m_errorCode;

  static auto composeMessage(const anvlk::types::RuntimeErrorMessage& msg, EGLint code)
    -> anvlk::types::RuntimeErrorString
  {
    return std::format("{} (EGL error: {} - 0x{:04X})", msg, describeError(code), code);
  }

  static auto describeError(EGLint code) -> std::string
  {
    switch (code)
    {
      case EGL_NOT_INITIALIZED:
        return "EGL_NOT_INITIALIZED";
      case EGL_BAD_ACCESS:
        return "EGL_BAD_ACCESS";
      case EGL_BAD_ALLOC:
        return "EGL_BAD_ALLOC";
      case EGL_BAD_ATTRIBUTE:
        return "EGL_BAD_ATTRIBUTE";
      case EGL_BAD_CONTEXT:
        return "EGL_BAD_CONTEXT";
      case EGL_BAD_CONFIG:
        return "EGL_BAD_CONFIG";
      case EGL_BAD_CURRENT_SURFACE:
        return "EGL_BAD_CURRENT_SURFACE";
      case EGL_BAD_DISPLAY:
        return "EGL_BAD_DISPLAY";
      case EGL_BAD_SURFACE:
        return "EGL_BAD_SURFACE";
      case EGL_BAD_MATCH:
        return "EGL_BAD_MATCH";
      case EGL_BAD_PARAMETER:
        return "EGL_BAD_PARAMETER";
      case EGL_BAD_NATIVE_PIXMAP:
        return "EGL_BAD_NATIVE_PIXMAP";
      case EGL_BAD_NATIVE_WINDOW:
        return "EGL_BAD_NATIVE_WINDOW";
      case EGL_CONTEXT_LOST:
        return "EGL_CONTEXT_LOST";
      default:
        return "UNKNOWN_EGL_ERROR";
    }
  }
};

} // namespace anvlk::render
