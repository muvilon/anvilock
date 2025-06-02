#ifndef ANVLK_RENDERER_GLUTILS_HPP
#define ANVLK_RENDERER_GLUTILS_HPP

#include <GLES3/gl3.h>
#include <anvilock/LogMacros.hpp>
#include <anvilock/Types.hpp>
#include <anvilock/shaders/ShaderHandler.hpp>
#include <vector>

namespace anvlk::render::GLUtils
{

inline constexpr GLuint GLRetCodeFail = 0;
// The fade out duration in seconds and steps
inline constexpr const int FadeSteps = 30;

namespace VBOBufStat
{
//@--> [ initialize the VBO buffer ] //
constexpr const GLsizei INIT = 0;
//@--> [ VBO buffer was NOT generated ] //
constexpr const GLsizei NOT_GEN = 0;
//@--> [ VBO buffer was generated ] //
constexpr const GLsizei GEN = 1;
} // namespace VBOBufStat

enum TextureStatus
{
  //@--> [ GL texture has been initialized ] //
  TEXTURE_INIT = 1,
  //@--> [ GL texture has been destroyed ] //
  TEXTURE_DESTROY = 0,
  //@--> [ GL texture is in UB state ] //
  UNDEF = -1
};

// //@--> [ GLUtil generic function to statically cast to <GLUINT> ] //
template <typename T> constexpr auto to_gluint(T value) -> GLuint
{
  return static_cast<GLuint>(value);
}

// //@--> [ GLUtil generic function to statically cast to <GLSIZEI> (which is INT) ] //
template <typename T> constexpr auto to_glsizei(T value) -> GLsizei
{
  return static_cast<GLsizei>(value);
}

inline auto checkShaderCompileStatus(GLuint shader, const anvlk::logger::LogContext& logCtx,
                                     const char* shaderName) -> bool
{
  GLint status = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_TRUE)
    return true;

  GLint logLen = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
  std::vector<GLchar> log(types::to_usize(logLen));
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
  std::vector<GLchar> log(types::to_usize(logLen));
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

inline void renderFadeQuad(float alpha, const logger::LogContext& logCtx,
                           anvlk::gfx::ShaderManager& shaderManager)
{
  static GLuint fadeVAO = 0;
  static GLuint fadeVBO = 0;

  if (fadeVAO == 0)
  {
    // Fullscreen quad vertices (NDC)
    constexpr const types::FloatArray<8> quadVertices = {
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    };

    glGenVertexArrays(1, &fadeVAO);
    glGenBuffers(1, &fadeVBO);

    glBindVertexArray(fadeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fadeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices.data(), GL_STATIC_DRAW);

    GLint posLoc = 0; // Use location 0 for position
    glEnableVertexAttribArray(GLUtils::to_gluint(posLoc));
    glVertexAttribPointer(GLUtils::to_gluint(posLoc), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          nullptr);

    glBindVertexArray(0);
  }

  // Simple shader program for fade quad (create once)
  static GLuint fadeShaderProgram =
    GLUtils::createShaderProgram<gfx::ShaderID::FADE_OUT_VERTEX, gfx::ShaderID::FADE_OUT_FRAG>(
      logCtx, shaderManager);
  glUseProgram(fadeShaderProgram);

  GLint alphaLoc = glGetUniformLocation(fadeShaderProgram, "uAlpha");
  glUniform1f(alphaLoc, alpha);

  glBindVertexArray(fadeVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  glUseProgram(0);
}

} // namespace anvlk::render::GLUtils

#endif
