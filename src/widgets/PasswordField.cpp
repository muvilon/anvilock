#include <anvilock/Types.hpp>
#include <anvilock/renderer/GLUtils.hpp>
#include <anvilock/widgets/PasswordField.hpp>
#include <anvilock/widgets/WidgetInterface.hpp>
#include <anvilock/widgets/WidgetRegistryMacros.hpp>

ANVLK_WIDGET_REGISTRY anvlk::types::WidgetName _PasswordFieldWidgetName;

namespace anvlk::widgets
{

auto renderPasswordField(ClientState& state) -> WidgetRegistryStatus
{
  using namespace render;

  GLuint program = GLUtils::createShaderProgram<gfx::ShaderID::RENDER_PWD_FIELD_EGL_VERTEX,
                                                gfx::ShaderID::RENDER_PWD_FIELD_EGL_FRAG>(
    state.logCtx, *state.shaderManagerPtr);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(program);

  // Uniform & attribute locations
  const GLint loc_color        = glGetUniformLocation(program, "color");
  const GLint loc_offset       = glGetUniformLocation(program, "offset");
  const GLint loc_scale        = glGetUniformLocation(program, "scale");
  const GLint loc_time         = glGetUniformLocation(program, "time");
  const GLint loc_cornerRadius = glGetUniformLocation(program, "cornerRadius");
  const GLint attr_position    = glGetAttribLocation(program, "position");

  const float fieldWidth   = 0.6f;
  const float fieldHeight  = 0.12f;
  const float cornerRadius = 0.06f;
  const float offsetX      = 0.0f;
  const float offsetY      = -0.75f + fieldHeight / 2.0f;

  const float timeSeconds =
    std::chrono::duration<float>(SteadyClock::now().time_since_epoch()).count();

  types::FloatArray<4> bgColor, borderColor, shadowColor;
  float                animationIntensity = 0.0f;

  const auto now          = SteadyClock::now();
  const auto glowDuration = PasswordFieldAnimation::GLOW_DURATION;
  const auto glowElapsed  = std::chrono::duration<float>(now - state.pwdFieldAnim.glowStartTime);
  const bool shouldGlow   = state.pwdFieldAnim.isGlowing && glowElapsed.count() < glowDuration;

  if (shouldGlow)
  {
    animationIntensity = 1.0f - glowElapsed.count() / glowDuration;

    bgColor = {0.95f + 0.05f * std::sin(timeSeconds * 8.0f) * animationIntensity,
               0.15f + 0.1f * animationIntensity, 0.15f + 0.1f * animationIntensity,
               0.92f + 0.08f * std::sin(timeSeconds * 4.0f) * animationIntensity};

    borderColor = {1.0f, 0.2f + 0.3f * animationIntensity, 0.2f + 0.3f * animationIntensity,
                   0.9f + 0.1f * animationIntensity};

    shadowColor = {1.0f, 0.0f, 0.0f, 0.4f * animationIntensity};
  }
  else if (state.pamState.authState.authFailed)
  {
    bgColor     = {0.98f, 0.94f, 0.94f, 0.95f};
    borderColor = {0.9f, 0.3f, 0.3f, 0.85f};
    shadowColor = {0.8f, 0.2f, 0.2f, 0.3f};
  }
  else
  {
    float breathe = 0.05f * std::sin(timeSeconds * 0.8f);
    bgColor       = {0.98f + breathe * 0.02f, 0.98f + breathe * 0.02f, 0.99f + breathe * 0.01f,
                     0.88f + breathe * 0.12f};
    borderColor   = {0.85f + breathe * 0.1f, 0.87f + breathe * 0.08f, 0.92f + breathe * 0.05f,
                     0.7f + breathe * 0.2f};
    shadowColor   = {0.0f, 0.0f, 0.0f, 0.08f + breathe * 0.04f};
  }

  const std::vector<float> fieldVerts = {-fieldWidth / 2,  -fieldHeight / 2, fieldWidth / 2,
                                         -fieldHeight / 2, fieldWidth / 2,   fieldHeight / 2,
                                         -fieldWidth / 2,  fieldHeight / 2};

  const auto drawField =
    [&](const types::FloatArray<4>& color, float ox, float oy, float sx, float sy)
  {
    glUniform4fv(loc_color, 1, color.data());
    glUniform2f(loc_offset, ox, oy);
    glUniform2f(loc_scale, sx, sy);
    glUniform1f(loc_cornerRadius, cornerRadius);
    glVertexAttribPointer(GLUtils::to_gluint(attr_position), 2, GL_FLOAT, GL_FALSE, 0,
                          fieldVerts.data());
    glEnableVertexAttribArray(GLUtils::to_gluint(attr_position));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  };

  drawField(shadowColor, offsetX + 0.008f, offsetY - 0.008f, 1.02f, 1.02f); // Drop shadow
  drawField(bgColor, offsetX, offsetY, 1.0f, 1.0f);                         // Background

  glUniform4fv(loc_color, 1, borderColor.data()); // Border
  glDrawArrays(GL_LINE_LOOP, 0, 4);

  // Draw password dots
  const auto dotCount = std::min(state.pamState.passwordIndex, types::to_usize(64)); // More dots
  if (dotCount > 0)
  {
    const float dotSpacing = 0.05f; // Fixed spacing between dots
    const float baseRadius = 0.015f;

    for (types::iters i = 0; i < dotCount; ++i)
    {
      float dotAge = timeSeconds - types::to_float(i) * 0.1f;
      float scale  = std::clamp(dotAge * 3.0f, 0.0f, 1.0f);
      float radius = baseRadius * scale;

      if (i == dotCount - 1)
        radius *= 1.0f + 0.15f * std::sin(timeSeconds * 6.0f);

      float x = offsetX - fieldWidth * 0.5f + types::to_float(i + 1) * dotSpacing;

      // Circle geometry
      constexpr const types::iters SEGMENTS    = 24;
      std::vector<float>           circleVerts = {0.0f, 0.0f};
      for (types::iters j = 0; j <= SEGMENTS; ++j)
      {
        auto angle = types::to_float(2.0 * M_PI * types::to_double(j) / SEGMENTS);
        circleVerts.push_back(radius * std::cos(angle));
        circleVerts.push_back(radius * std::sin(angle));
      }

      // Shadow
      const types::FloatArray<4> dotShadowColor = {0.0f, 0.0f, 0.0f, 0.25f};
      glUniform4fv(loc_color, 1, dotShadowColor.data());
      glUniform2f(loc_offset, x + 0.004f, offsetY - 0.004f);
      glVertexAttribPointer(GLUtils::to_gluint(attr_position), 2, GL_FLOAT, GL_FALSE, 0,
                            circleVerts.data());
      glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS + 2);

      // Dot color with subtle animation
      float                gradient = 0.95f + 0.05f * (float(i) / float(dotCount));
      types::FloatArray<4> dotColor = {(0.25f * gradient) + 0.05f * std::sin(timeSeconds * 2.0f),
                                       (0.25f * gradient) + 0.05f * std::sin(timeSeconds * 2.2f),
                                       (0.35f * gradient) + 0.05f * std::sin(timeSeconds * 1.8f),
                                       0.85f + 0.15f * std::sin(timeSeconds * 1.5f)};
      glUniform4fv(loc_color, 1, dotColor.data());
      glUniform2f(loc_offset, x, offsetY);
      glVertexAttribPointer(GLUtils::to_gluint(attr_position), 2, GL_FLOAT, GL_FALSE, 0,
                            circleVerts.data());
      glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS + 2);
    }
  }

  if (loc_time != -1)
    glUniform1f(loc_time, timeSeconds);

  glDisable(GL_BLEND);
  glDeleteProgram(program);

  return {0, true};
}

REGISTER_WIDGET(_PasswordFieldWidgetName, renderPasswordField);

} // namespace anvlk::widgets
