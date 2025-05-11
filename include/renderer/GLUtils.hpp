#ifndef ANVLK_RENDERER_GLUTILS_HPP
#define ANVLK_RENDERER_GLUTILS_HPP

#include <GLES2/gl2.h>
#include <anvilock/include/LogMacros.hpp>
#include <anvilock/include/shaders/ShaderHandler.hpp>
#include <vector>

namespace anvlk::render::GLUtils
{
inline auto checkShaderCompileStatus(GLuint shader, const anvlk::logger::LogContext& logCtx,
                                     const char* shaderName) -> bool
{
  GLint status = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_TRUE)
    return true;

  GLint logLen = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
  std::vector<GLchar> log(logLen);
  glGetShaderInfoLog(shader, logLen, nullptr, log.data());

  LOG::ERROR(logCtx, "{} shader compilation failed: {}", shaderName, log.data());
  return false;
}

inline auto checkProgramLinkStatus(GLuint program, const anvlk::logger::LogContext& logCtx) -> bool
{
  GLint status = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_TRUE)
    return true;

  GLint logLen = 0;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
  std::vector<GLchar> log(logLen);
  glGetProgramInfoLog(program, logLen, nullptr, log.data());

  LOG::ERROR(logCtx, "Shader program linking failed: {}", log.data());
  return false;
}

template <anvlk::gfx::ShaderID VertexID, anvlk::gfx::ShaderID FragmentID>
static auto createShaderProgram(const logger::LogContext&  logCtx,
                                anvlk::gfx::ShaderManager& shaderManager) -> GLuint
{
  const auto vertexOpt   = shaderManager.getShaderSource(VertexID);
  const auto fragmentOpt = shaderManager.getShaderSource(FragmentID);

  if (!(vertexOpt && fragmentOpt))
    return types::EGLCodes::RET_CODE_FAIL;

  types::ShaderContentCStr vertexSrc   = shaderManager.asCString(vertexOpt);
  types::ShaderContentCStr fragmentSrc = shaderManager.asCString(fragmentOpt);

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
  glCompileShader(vertexShader);
  if (!checkShaderCompileStatus(vertexShader, logCtx, "Vertex"))
  {
    glDeleteShader(vertexShader);
    return types::EGLCodes::RET_CODE_FAIL;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
  glCompileShader(fragmentShader);
  if (!checkShaderCompileStatus(fragmentShader, logCtx, "Fragment"))
  {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return types::EGLCodes::RET_CODE_FAIL;
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  if (!checkProgramLinkStatus(program, logCtx))
  {
    glDeleteProgram(program);
    return types::EGLCodes::RET_CODE_FAIL;
  }

  return program;
}

} // namespace anvlk::render::GLUtils

#endif
