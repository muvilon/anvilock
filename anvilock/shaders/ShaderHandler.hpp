#ifndef ANVLK_SHADERS_SHADER_MANAGER_HPP
#define ANVLK_SHADERS_SHADER_MANAGER_HPP

#include <anvilock/LogMacros.hpp>
#include <anvilock/Types.hpp>
#include <optional>
#include <unordered_map>

namespace anvlk::gfx
{

inline constexpr const std::string_view GLOBAL_SHADER_DIR    = "/usr/share/anvilock/shaders/";
inline constexpr const std::string_view REL_LOCAL_SHADER_DIR = ".local/share/anvilock/shaders/";

#define SHADER_ID_LIST            \
  X(INIT_EGL_VERTEX)              \
  X(INIT_EGL_FRAG)                \
  X(RENDER_PWD_FIELD_EGL_VERTEX)  \
  X(RENDER_PWD_FIELD_EGL_FRAG)    \
  X(RENDER_TIME_FIELD_EGL_VERTEX) \
  X(RENDER_TIME_FIELD_EGL_FRAG)   \
  X(TEXTURE_EGL_VERTEX)           \
  X(TEXTURE_EGL_FRAG)             \
  X(FADE_OUT_VERTEX)              \
  X(FADE_OUT_FRAG)                \
  X(BLUR_VERTEX)                  \
  X(BLUR_FRAG)                    \
  X(SHADOW_VERTEX)                \
  X(SHADOW_FRAG)

enum class ShaderID
{
#define X(x) x,
  SHADER_ID_LIST
#undef X
};

inline constexpr auto shaderIDToStr(ShaderID id) -> types::ShaderName
{
  switch (id)
  {
#define X(x)        \
  case ShaderID::x: \
    return #x;
    SHADER_ID_LIST
#undef X
    default:
      return "UNKNOWN_SHADER_ID";
  }
}

inline const std::unordered_map<ShaderID, const std::string_view> ShaderPaths = {
  {ShaderID::INIT_EGL_VERTEX, "egl/init/vertex_shader.glsl"},
  {ShaderID::INIT_EGL_FRAG, "egl/init/fragment_shader.glsl"},
  {ShaderID::RENDER_PWD_FIELD_EGL_VERTEX, "egl/render_password_field/vertex_shader.glsl"},
  {ShaderID::RENDER_PWD_FIELD_EGL_FRAG, "egl/render_password_field/fragment_shader.glsl"},
  {ShaderID::RENDER_TIME_FIELD_EGL_VERTEX, "egl/render_time_box/vertex_shader.glsl"},
  {ShaderID::RENDER_TIME_FIELD_EGL_FRAG, "egl/render_time_box/fragment_shader.glsl"},
  {ShaderID::TEXTURE_EGL_VERTEX, "egl/texture/vertex_shader.glsl"},
  {ShaderID::TEXTURE_EGL_FRAG, "egl/texture/fragment_shader.glsl"},
  {ShaderID::FADE_OUT_VERTEX, "egl/fadeout/vertex_shader.glsl"},
  {ShaderID::FADE_OUT_FRAG, "egl/fadeout/fragment_shader.glsl"},
  {ShaderID::SHADOW_VERTEX, "egl/shadow/vertex_shader.glsl"},
  {ShaderID::SHADOW_FRAG, "egl/shadow/fragment_shader.glsl"},
  {ShaderID::BLUR_VERTEX, "egl/blur/vertex_shader.glsl"},
  {ShaderID::BLUR_FRAG, "egl/blur/fragment_shader.glsl"},
};

class ShaderManager
{
public:
  ShaderManager(const types::Directory& homeDir, logger::LogContext& ctx);

  [[nodiscard]] auto isValid() const -> bool;
  [[nodiscard]] auto getShaderSource(ShaderID id) const -> std::optional<types::ShaderContent>;
  [[nodiscard]] static auto asCString(const std::optional<types::ShaderContent>& content)
    -> types::ShaderContentCStr;

private:
  types::PathCStr                                    m_homePath;
  logger::LogContext&                                m_ctx;
  std::optional<types::fsPath>                       m_shaderDir;
  std::unordered_map<ShaderID, types::ShaderContent> m_shaders;

  [[nodiscard]] auto findShaderRuntimeDir() const -> std::optional<types::fsPath>;
  void               loadAllShaders();
  [[nodiscard]] auto loadShaderSource(const types::fsPath& filePath) const
    -> std::optional<types::Path>;
};

} // namespace anvlk::gfx

#endif
