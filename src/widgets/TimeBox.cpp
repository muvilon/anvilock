#include <anvilock/include/renderer/GLUtils.hpp>
#include <anvilock/include/widgets/TimeBox.hpp>

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

void renderTimeBox(ClientState& cs)
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

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        (void*)(2 * sizeof(GLfloat)));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, cs.timeTexture);

  glUseProgram(cs.shaderState.textureShaderProgram);
  glUniform1i(glGetUniformLocation(cs.shaderState.textureShaderProgram, "uTexture"), 0);

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

} // namespace anvlk::widgets
