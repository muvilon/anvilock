#include <GLES3/gl3.h>
#include <anvilock/Types.hpp>
#include <anvilock/renderer/GLUtils.hpp>
#include <anvilock/widgets/PasswordField.hpp>
#include <anvilock/widgets/WidgetInterface.hpp>
#include <anvilock/widgets/WidgetRegistryMacros.hpp>
#include <anvilock/widgets/helpers/Renderable.hpp>
#include <anvilock/widgets/helpers/Shapes.hpp>
#include <cmath>
#include <memory>

ANVLK_WIDGET_REGISTRY anvlk::types::WidgetName _PasswordFieldWidgetName;

namespace anvlk::widgets
{

auto renderPasswordField(ClientState& state) -> WidgetRegistryStatus
{

  GLuint program = render::GLUtils::createShaderProgram<gfx::ShaderID::RENDER_PWD_FIELD_EGL_VERTEX,
                                                        gfx::ShaderID::RENDER_PWD_FIELD_EGL_FRAG>(
    state.logCtx, *state.shaderManagerPtr);

  ANVLK_USE_GL_PROGRAM_WITH_BLEND(program);

  const GLint loc_color        = glGetUniformLocation(program, "color");
  const GLint loc_offset       = glGetUniformLocation(program, "offset");
  const GLint loc_scale        = glGetUniformLocation(program, "scale");
  const GLint loc_time         = glGetUniformLocation(program, "time");
  const GLint loc_cornerRadius = glGetUniformLocation(program, "cornerRadius");
  const GLint attr_position    = glGetAttribLocation(program, "position");

  const float fieldWidth   = types::to_float(state.userConfig.pwdFieldCfg.width);
  const float fieldHeight  = types::to_float(state.userConfig.pwdFieldCfg.height);
  const GLint loc_halfSize = glGetUniformLocation(program, "halfSize");
  glUniform2f(loc_halfSize, fieldWidth / 2.0f, fieldHeight / 2.0f);

  const float cornerRadius = 0.06f;
  const float offsetX      = 0.0f;
  const float offsetY      = -0.75f + fieldHeight / 2.0f;

  const float timeSeconds =
    std::chrono::duration<float>(SteadyClock::now().time_since_epoch()).count();

  types::FloatArray<4> bgColor;
  const auto           now          = SteadyClock::now();
  const auto           glowDuration = PasswordFieldAnimation::GLOW_DURATION;
  const auto glowElapsed = std::chrono::duration<float>(now - state.pwdFieldAnim.glowStartTime);
  const bool shouldGlow  = state.pwdFieldAnim.isGlowing && glowElapsed.count() < glowDuration;

  if (shouldGlow)
  {
    const float intensity = 1.0f - glowElapsed.count() / glowDuration;
    bgColor = {0.9f + 0.1f * std::sin(timeSeconds * 8.0f) * intensity, 0.2f + 0.1f * intensity,
               0.2f + 0.1f * intensity, 0.92f + 0.08f * std::sin(timeSeconds * 4.0f) * intensity};
  }
  else if (state.pamState.authState.authFailed)
  {
    bgColor = PASSWORD_FAIL_BG_COLOR;
  }
  else
  {
    const float breathe = 0.05f * std::sin(timeSeconds * 0.8f);
    bgColor = {0.98f + breathe * 0.02f, 0.98f + breathe * 0.02f, 0.99f + breathe * 0.01f,
               0.88f + breathe * 0.12f};
  }

  std::vector<std::unique_ptr<helpers::Renderable>> renderables;

  // Shadow pass
  renderables.push_back(std::make_unique<helpers::RoundedRect>(
    state.userConfig.pwdFieldCfg.shadowColor, offsetX + 0.007f, offsetY - 0.007f, 1.04f, 1.04f,
    cornerRadius, fieldWidth, fieldHeight));

  // Main field
  renderables.push_back(std::make_unique<helpers::RoundedRect>(
    bgColor, offsetX, offsetY, 1.0f, 1.0f, cornerRadius, fieldWidth, fieldHeight));

  // Password dots
  const auto dotCount = utils::min(state.pamState.passwordIndex, types::to_usize(11));
  if (dotCount > 0)
  {
    const float dotSpacing = 0.05f;
    const float baseRadius = 0.015f;

    for (types::iters i = 0; i < dotCount; ++i)
    {
      float dotAge = timeSeconds - types::to_float(i) * 0.1f;
      float scale  = std::clamp(dotAge * 3.0f, 0.0f, 1.0f);
      float radius = baseRadius * scale;

      if (i == dotCount - 1)
        radius *= 1.0f + 0.15f * std::sin(timeSeconds * 6.0f);

      float x = offsetX - fieldWidth * 0.5f + types::to_float(i + 1) * dotSpacing;

      types::FloatArray<4> dotColor = {0.2f + 0.05f * std::sin(timeSeconds * 2.0f),
                                       0.2f + 0.05f * std::sin(timeSeconds * 2.1f),
                                       0.3f + 0.05f * std::sin(timeSeconds * 2.2f), 0.9f};

      renderables.push_back(std::make_unique<helpers::Circle>(dotColor, x, offsetY, radius));
    }
  }

  // Draw all renderables
  for (const auto& r : renderables)
  {
    r->update(0.0f); // optional animation
    r->draw(loc_color, loc_offset, loc_scale, loc_cornerRadius, loc_time, attr_position,
            timeSeconds);
  }

  ANVLK_DELETE_GL_PROGRAM_WITH_BLEND(program);
  return {0, true};
}

REGISTER_WIDGET(_PasswordFieldWidgetName, renderPasswordField);

} // namespace anvlk::widgets
