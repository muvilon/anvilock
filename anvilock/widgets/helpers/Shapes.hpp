#pragma once

#include <anvilock/Types.hpp>
#include <anvilock/renderer/GLUtils.hpp>
#include <anvilock/widgets/helpers/Renderable.hpp>
#include <cmath>
#include <vector>

namespace anvlk::widgets::helpers
{

class RoundedRect : public Renderable
{
public:
  RoundedRect(const types::FloatArray<4>& inputColor, float inputOffsetX, float inputOffsetY,
              float inputScaleX, float inputScaleY, float inputCornerRadius, float inputWidth,
              float inputHeight)
      : cornerRadius(inputCornerRadius), width(inputWidth), height(inputHeight)
  {
    setColor(inputColor);
    setOffset(inputOffsetX, inputOffsetY);
    setScale(inputScaleX, inputScaleY);
  }

  void draw(GLint loc_color, GLint loc_offset, GLint loc_scale, GLint loc_cornerRadius,
            GLint loc_time, GLint attr_position, float timeSeconds) const override
  {
    if (!isVisible())
      return;

    const std::vector<float> verts = {
      -width / 2, -height / 2, width / 2,  -height / 2,
      width / 2,  height / 2,  -width / 2, height / 2,
    };

    types::FloatArray<4> finalColor = {color_[0], color_[1], color_[2], color_[3] * opacity_};

    glUniform4fv(loc_color, 1, finalColor.data());
    glUniform2f(loc_offset, offsetX_, offsetY_);
    glUniform2f(loc_scale, scaleX_, scaleY_);
    glUniform1f(loc_cornerRadius, cornerRadius);
    glUniform1f(loc_time, timeSeconds);

    glVertexAttribPointer(render::GLUtils::to_gluint(attr_position), 2, GL_FLOAT, GL_FALSE, 0,
                          verts.data());
    glEnableVertexAttribArray(render::GLUtils::to_gluint(attr_position));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }

  void update(float /*deltaTime*/) override {}

  [[nodiscard]] auto clone() const -> Renderable* override
  {
    return new RoundedRect(color_, offsetX_, offsetY_, scaleX_, scaleY_, cornerRadius, width,
                           height);
  }

  void setSize(float w, float h)
  {
    width  = w;
    height = h;
  }

  void setCornerRadius(float r) { cornerRadius = r; }

  [[nodiscard]] auto getSize() const -> std::pair<float, float> { return {width, height}; }

private:
  float cornerRadius;
  float width, height;
};

class Circle : public Renderable
{
public:
  Circle(const types::FloatArray<4>& inputColor, float inputX, float inputY, float inputRadius)
      : radius(inputRadius)
  {
    setColor(inputColor);
    setOffset(inputX, inputY);
  }

  void draw(GLint loc_color, GLint loc_offset, GLint loc_scale, GLint loc_cornerRadius,
            GLint loc_time, GLint attr_position, float timeSeconds) const override
  {
    if (!isVisible())
      return;

    constexpr int      SEGMENTS = 24;
    std::vector<float> verts    = {0.0f, 0.0f};

    for (int j = 0; j <= SEGMENTS; ++j)
    {
      float angle = types::to_float(2.0 * M_PI * j / SEGMENTS);
      verts.push_back(radius * std::cos(angle));
      verts.push_back(radius * std::sin(angle));
    }

    types::FloatArray<4> finalColor = {color_[0], color_[1], color_[2], color_[3] * opacity_};

    glUniform4fv(loc_color, 1, finalColor.data());
    glUniform2f(loc_offset, offsetX_, offsetY_);
    glUniform2f(loc_scale, 1.0f, 1.0f);
    glUniform1f(loc_cornerRadius, 0.0f);
    glUniform1f(loc_time, timeSeconds);

    glVertexAttribPointer(render::GLUtils::to_gluint(attr_position), 2, GL_FLOAT, GL_FALSE, 0,
                          verts.data());
    glEnableVertexAttribArray(render::GLUtils::to_gluint(attr_position));
    glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS + 2);
  }

  void update(float /*deltaTime*/) override {}

  [[nodiscard]] auto clone() const -> Renderable* override
  {
    return new Circle(color_, offsetX_, offsetY_, radius);
  }

  void               setRadius(float r) { radius = r; }
  [[nodiscard]] auto getRadius() const -> float { return radius; }

private:
  float radius;
};

} // namespace anvlk::widgets::helpers
