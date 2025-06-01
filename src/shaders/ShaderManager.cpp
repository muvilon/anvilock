#include <anvilock/shaders/ShaderHandler.hpp>
#include <fstream>
#include <sstream>

namespace anvlk::gfx
{

auto shaderIDToStr(ShaderID id) -> types::ShaderName
{
  switch (id)
  {
    case ShaderID::INIT_EGL_VERTEX:
      return "INIT_EGL_VERTEX";
    case ShaderID::INIT_EGL_FRAG:
      return "INIT_EGL_FRAG";
    case ShaderID::RENDER_PWD_FIELD_EGL_VERTEX:
      return "RENDER_PWD_FIELD_EGL_VERTEX";
    case ShaderID::RENDER_PWD_FIELD_EGL_FRAG:
      return "RENDER_PWD_FIELD_EGL_FRAG";
    case ShaderID::RENDER_TIME_FIELD_EGL_VERTEX:
      return "RENDER_TIME_FIELD_EGL_VERTEX";
    case ShaderID::RENDER_TIME_FIELD_EGL_FRAG:
      return "RENDER_TIME_FIELD_EGL_FRAG";
    case ShaderID::TEXTURE_EGL_VERTEX:
      return "TEXTURE_EGL_VERTEX";
    case ShaderID::TEXTURE_EGL_FRAG:
      return "TEXTURE_EGL_FRAG";
    default:
      return "UNKNOWN_SHADER_ID";
  }
}

ShaderManager::ShaderManager(const types::Directory& homeDir, logger::LogContext& ctx)
    : m_homePath(homeDir.c_str()), m_ctx(ctx)
{
  m_shaderDir = findShaderRuntimeDir();
  if (m_shaderDir)
    loadAllShaders();
}

[[nodiscard]] auto ShaderManager::isValid() const -> bool { return m_shaderDir.has_value(); }

[[nodiscard]] auto ShaderManager::getShaderSource(ShaderID id) const
  -> std::optional<types::ShaderContent>
{
  if (auto it = m_shaders.find(id); it != m_shaders.end())
  {
    return it->second;
  }
  return std::nullopt;
}

[[nodiscard]] auto ShaderManager::findShaderRuntimeDir() const -> std::optional<types::fsPath>
{
  types::fsPath globalPath = GLOBAL_SHADER_DIR;
  if (std::filesystem::exists(globalPath))
  {
    LOG::INFO(m_ctx, logger::LogStyle::COLOR_BOLD, "Found GLOBAL SHADER RUNTIME AT: {}",
              globalPath.c_str());
    return globalPath;
  }

  types::fsPath localPath = types::fsPath(m_homePath) / REL_LOCAL_SHADER_DIR;

  LOG::TRACE(m_ctx, "Attempting to find LOCAL SHADER RUNTIME at: {} from '{}'", localPath.c_str(),
             m_homePath);

  if (std::filesystem::exists(localPath))
  {
    LOG::INFO(m_ctx, logger::LogStyle::COLOR_BOLD, "Found LOCAL SHADER RUNTIME AT: {}",
              localPath.c_str());
    return localPath;
  }

  LOG::ERROR(m_ctx, logger::LogStyle::COLOR_BOLD, "Shader runtime directory not found!");
  return std::nullopt;
}

void ShaderManager::loadAllShaders()
{
  for (const auto& [id, relPath] : ShaderPaths)
  {
    types::fsPath fullPath = *m_shaderDir / relPath;
    LOG::TRACE(m_ctx, "Loading SHADER: '{}'", shaderIDToStr(id), fullPath.c_str());
    LOG::TRACE(m_ctx, "SHADER path: '{}'", fullPath.c_str());

    if (auto source = loadShaderSource(fullPath))
    {
      m_shaders[id] = std::move(*source);
      LOG::INFO(m_ctx, logger::LogStyle::COLOR_BOLD, "Loaded SHADER: '{}'", shaderIDToStr(id),
                fullPath.c_str());
    }
    else
    {
      LOG::ERROR(m_ctx, logger::LogStyle::COLOR_BOLD, "Failed to load SHADER: '{}'",
                 shaderIDToStr(id), fullPath.c_str());
    }
  }
}

[[nodiscard]] auto ShaderManager::loadShaderSource(const types::fsPath& filePath) const
  -> std::optional<types::Path>
{
  std::ifstream file(filePath, std::ios::in | std::ios::binary);
  if (!file)
  {
    LOG::ERROR(m_ctx, logger::LogStyle::COLOR_BOLD, "Failed to open shader file: {}",
               filePath.string());
    return std::nullopt;
  }

  std::ostringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

[[nodiscard]] auto ShaderManager::asCString(const std::optional<types::ShaderContent>& content)
  -> types::ShaderContentCStr
{
  return content ? content->c_str() : nullptr;
}

} // namespace anvlk::gfx
