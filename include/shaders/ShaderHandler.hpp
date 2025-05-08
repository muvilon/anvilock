#pragma once

#include <anvilock/include/LogMacros.hpp>
#include <anvilock/include/Types.hpp>
#include <optional>
#include <unordered_map>

namespace anvlk::gfx
{

inline constexpr std::string_view GLOBAL_SHADER_DIR    = "/usr/share/anvilock/shaders/";
inline constexpr std::string_view REL_LOCAL_SHADER_DIR = ".local/share/anvilock/shaders/";

enum class ShaderID
{
  INIT_EGL_VERTEX,
  INIT_EGL_FRAG,
  RENDER_PWD_FIELD_EGL_VERTEX,
  RENDER_PWD_FIELD_EGL_FRAG,
  RENDER_TIME_FIELD_EGL_VERTEX,
  RENDER_TIME_FIELD_EGL_FRAG,
  TEXTURE_EGL_VERTEX,
  TEXTURE_EGL_FRAG,
};

inline auto shaderIDToStr(ShaderID id) -> types::ShaderName;

inline const std::unordered_map<ShaderID, std::string_view> ShaderPaths = {
  {ShaderID::INIT_EGL_VERTEX, "egl/init/vertex_shader.glsl"},
  {ShaderID::INIT_EGL_FRAG, "egl/init/fragment_shader.glsl"},
  {ShaderID::RENDER_PWD_FIELD_EGL_VERTEX, "egl/render_password_field/vertex_shader.glsl"},
  {ShaderID::RENDER_PWD_FIELD_EGL_FRAG, "egl/render_password_field/fragment_shader.glsl"},
  {ShaderID::RENDER_TIME_FIELD_EGL_VERTEX, "egl/render_time_box/vertex_shader.glsl"},
  {ShaderID::RENDER_TIME_FIELD_EGL_FRAG, "egl/render_time_box/fragment_shader.glsl"},
  {ShaderID::TEXTURE_EGL_VERTEX, "egl/texture/vertex_shader.glsl"},
  {ShaderID::TEXTURE_EGL_FRAG, "egl/texture/fragment_shader.glsl"},
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
