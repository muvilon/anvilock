#include <anvilock/renderer/GLUtils.hpp>
#include <anvilock/widgets/TimeBox.hpp>

namespace anvlk::widgets
{

namespace timebox
{
void updateTimeTexture(ClientState& cs)
{
  types::TimeString timeStr = utils::getTimeString(cs.userConfig.time.time_format);

  static types::TimeString lastRecordedTimeStr = "";

  if (lastRecordedTimeStr != timeStr)
  {
    LOG::TRACE(cs.logCtx, "Time changed from '{}' to '{}'", lastRecordedTimeStr, timeStr);
    lastRecordedTimeStr = timeStr;

    if (cs.timeTexture)
    {
      // 1 is the texture count to be deleted
      glDeleteTextures(1, &cs.timeTexture);
      cs.timeTexture = render::GLUtils::TextureStatus::TEXTURE_DESTROY;
      LOG::TRACE(cs.logCtx, "Old time texture deleted.");
    }

    // rewrites the time texture with respect to the new time
    cs.timeTexture = widgets::helpers::createTextTexture(cs, timeStr);
  }
}
} // namespace timebox

void renderTimeBox(ClientState& cs, float fadeAlpha)
{
  timebox::updateTimeTexture(cs);
  if (cs.timeTexture == render::GLUtils::TextureStatus::TEXTURE_DESTROY)
  {
    LOG::ERROR(cs.logCtx, "No valid texture for rendering.");
    return;
  }

  static GLuint VBO = render::GLUtils::VBOBufStat::INIT;
  if (VBO == render::GLUtils::VBOBufStat::NOT_GEN)
  {
    glGenBuffers(render::GLUtils::VBOBufStat::GEN, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cs.userConfig.timeBoxVertices),
                 cs.userConfig.timeBoxVertices.data(), GL_STATIC_DRAW);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Use custom shader for rounded time box
  static GLuint shader =
    render::GLUtils::createShaderProgram<gfx::ShaderID::RENDER_TIME_FIELD_EGL_VERTEX,
                                         gfx::ShaderID::RENDER_TIME_FIELD_EGL_FRAG>(
      cs.logCtx, *cs.shaderManagerPtr);

  glUseProgram(shader);

  GLint texLoc    = glGetUniformLocation(shader, "uTexture");
  GLint alphaLoc  = glGetUniformLocation(shader, "uAlpha");
  GLint radiusLoc = glGetUniformLocation(shader, "uRadius");

  glUniform1i(texLoc, 0);
  glUniform1f(alphaLoc, fadeAlpha);
  glUniform1f(radiusLoc, 0.15f); // adjust for look — 0.1 to 0.2 is nice

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        (void*)(2 * sizeof(GLfloat)));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, cs.timeTexture);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
  glDisable(GL_BLEND);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    LOG::ERROR(cs.logCtx, "OpenGL error when rendering time box: {}", error);
  }

  LOG::TRACE(cs.logCtx, "Modern time box rendered successfully!");
}

} // namespace anvlk::widgets
