#ifndef ANVLK_WIDGETS_HELPERS_RENDERABLE_INTERFACE_HPP
#define ANVLK_WIDGETS_HELPERS_RENDERABLE_INTERFACE_HPP

#include <GLES3/gl3.h>
#include <anvilock/Types.hpp>

namespace anvlk::widgets::helpers
{

/// A reusable Renderable interface for OpenGL ES 3.0-based UI components.
class Renderable
{
public:
  virtual ~Renderable() = default;

  /// Render the component using the active shader program.
  virtual void draw(GLint loc_color, GLint loc_offset, GLint loc_scale, GLint loc_cornerRadius,
                    GLint loc_time, GLint attr_position, float timeSeconds) const = 0;

  /// Called every frame with time delta; override to animate.
  virtual void update(float deltaTime) = 0;

  /// Set visibility; invisible components won't draw.
  virtual void setVisible(bool visible) { isVisible_ = visible; }

  /// Check if the component is currently visible.
  [[nodiscard]] virtual auto isVisible() const -> bool { return isVisible_; }

  /// Set global alpha (used for fade or tint).
  virtual void setOpacity(float alpha) { opacity_ = alpha; }

  /// Get global opacity.
  [[nodiscard]] virtual auto getOpacity() const -> float { return opacity_; }

  /// Set transform scale.
  virtual void setScale(float sx, float sy)
  {
    scaleX_ = sx;
    scaleY_ = sy;
  }

  /// Set transform offset (position).
  virtual void setOffset(float ox, float oy)
  {
    offsetX_ = ox;
    offsetY_ = oy;
  }

  /// Set base color (RGBA).
  virtual void setColor(const types::FloatArray<4>& color) { color_ = color; }

  /// Optional clone (useful for retained UI graphs).
  [[nodiscard]] virtual auto clone() const -> Renderable* { return nullptr; }

protected:
  float offsetX_ = 0.0f;
  float offsetY_ = 0.0f;
  float scaleX_  = 1.0f;
  float scaleY_  = 1.0f;
  float opacity_ = 1.0f;

  types::FloatArray<4> color_     = {1.0f, 1.0f, 1.0f, 1.0f};
  bool                 isVisible_ = true;
};

} // namespace anvlk::widgets::helpers

#endif // ANVLK_WIDGETS_HELPERS_RENDERABLE_INTERFACE_HPP
