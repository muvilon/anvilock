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
  GLuint program =
    render::GLUtils::createShaderProgram<anvlk::gfx::ShaderID::RENDER_PWD_FIELD_EGL_VERTEX,
                                         anvlk::gfx::ShaderID::RENDER_PWD_FIELD_EGL_FRAG>(
      state.logCtx, *state.shaderManagerPtr);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Use the shader program
  glUseProgram(program);

  // Get uniform locations
  GLint color_location    = glGetUniformLocation(program, "color");
  GLint offset_location   = glGetUniformLocation(program, "offset");
  GLint position_location = glGetAttribLocation(program, "position");

  // Width and height of the password field
  float field_width  = 0.7f;  // Adjusted width for the field
  float field_height = 0.15f; // Adjusted height for the field

  // Position offset to center at the bottom of the screen
  float offset_x = 0;                           // Horizontally center the field
  float offset_y = -0.8f + field_height / 2.0f; // Vertically align it at the bottom

  // Determine background and border colors
  types::FloatArray<4> bgColor, borderColor;

  // Check if we're in a glow state
  TimePoint currentTime = SteadyClock::now();
  auto      timeSinceGlowStart =
    std::chrono::duration<float>(currentTime - state.pwdFieldAnim.glowStartTime);
  bool shouldGlow = state.pwdFieldAnim.isGlowing &&
                    timeSinceGlowStart.count() < PasswordFieldAnimation::GLOW_DURATION;

  if (shouldGlow)
  {
    // Calculate glow intensity (fade out over time)
    float glowIntensity =
      1.0f - (timeSinceGlowStart.count() / PasswordFieldAnimation::GLOW_DURATION);

    // Interpolate between normal and error colors
    bgColor = {
      1.0f,                        // R
      0.3f + 0.7f * glowIntensity, // G (fades from white to red)
      0.3f + 0.7f * glowIntensity, // B (fades from white to red)
      0.7f                         // Alpha
    };
    borderColor = {
      1.0f,                 // R
      0.0f + glowIntensity, // G (fades from red border to white)
      0.0f + glowIntensity, // B (fades from red border to white)
      1.0f                  // Alpha
    };

    // Stop glowing if duration has passed
    if (!shouldGlow)
    {
      state.pwdFieldAnim.isGlowing = false;
    }
  }
  else if (state.pamState.authState.authFailed)
  {
    // Static red for authentication failure
    bgColor     = {1.0f, 0.3f, 0.3f, 0.5f}; // Lighter transparent red background
    borderColor = {1.0f, 0.0f, 0.0f, 1.0f}; // Solid red border
  }
  else
  {
    // Normal colors for successful or ongoing authentication
    bgColor     = {1.0f, 1.0f, 1.0f, 0.70f}; // Light background with transparency
    borderColor = {0.9f, 0.9f, 0.9f, 0.8f};  // Subtle border
  }

  // Set background color
  glUniform4fv(color_location, 1, bgColor.data());
  glUniform2f(offset_location, offset_x, offset_y);
  glVertexAttribPointer(render::GLUtils::to_gluint(position_location), 2, GL_FLOAT, GL_FALSE, 0,
                        utils::password_field_vertices.data());
  glEnableVertexAttribArray(render::GLUtils::to_gluint(position_location));

  // Draw the background of the password field
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Draw the border
  glUniform4fv(color_location, 1, borderColor.data());
  glDrawArrays(GL_LINE_LOOP, 0, 4);

  // Draw password dots as circles
  glUniform4f(color_location, 0.3f, 0.3f, 0.3f, 0.8f); // Gray dots

  // Adjust dot positions based on password input
  float dot_spacing = field_width / types::to_float(state.pamState.passwordIndex + 1);
  float dot_radius  = 0.02f; // Radius of each dot

  for (types::iters i = 0; i < state.pamState.passwordIndex; i++)
  {
    float x_position =
      offset_x + types::to_float(i + 1) * dot_spacing - field_width / 2; // Center the dots

    // Generate circle vertices
    std::vector<float> circle_vertices;
    types::iters       num_segments = 32; // Number of triangle fan segments for smooth circle
    for (types::iters j = 0; j <= num_segments; ++j)
    {
      double angle_d = (2.0 * M_PI * types::to_double(j)) / types::to_double(num_segments);
      float  angle   = types::to_float(angle_d);
      circle_vertices.push_back(dot_radius * std::cos(angle));
      circle_vertices.push_back(dot_radius * std::sin(angle));
    }

    // Set uniform for dot position
    glUniform2f(offset_location, x_position, offset_y);

    // Draw circle using triangle fan
    glVertexAttribPointer(render::GLUtils::to_gluint(position_location), 2, GL_FLOAT, GL_FALSE, 0,
                          circle_vertices.data());
    ANVLK_ASSERT(num_segments + 2 <= std::numeric_limits<GLsizei>::max());
    glDrawArrays(GL_TRIANGLE_FAN, 0, render::GLUtils::to_glsizei(num_segments + 2));
  }

  // Disable blending and clean up
  glDisable(GL_BLEND);
  glDeleteProgram(program);

  return {0, true};
}

REGISTER_WIDGET(_PasswordFieldWidgetName, renderPasswordField);

} // namespace anvlk::widgets
