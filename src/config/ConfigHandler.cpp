#include <anvilock/LogMacros.hpp>
#include <anvilock/config/ConfigHandler.hpp>
#include <stdexcept>

namespace anvlk::cfg
{

auto ConfigLoader::getUserConfigPath() -> types::fsPath
{
  if (!m_homeDir)
    throw std::runtime_error("No HOME env var found!");
  return anvlk::types::fsPath(m_homeDir) / REL_CFG_PATH / CFG_FILE_NAME;
}

ConfigLoader::ConfigLoader(anvlk::logger::LogContext& logCtx, types::Directory& homeDir,
                           const types::fsPath& customPath)
    : m_logCtx(logCtx), m_homeDir(std::move(homeDir.c_str())),
      m_configPath(customPath.empty() ? getUserConfigPath() : customPath)

{
}

auto ConfigLoader::getConfigPath() const -> types::fsPath { return m_configPath; }

auto ConfigLoader::parseVec2(const toml::array* arr) -> anvlk::types::FloatArray<2>
{
  if (!arr || arr->size() != 2)
    return {0.0f, 0.0f};

  auto getVal = [](const toml::node& node) -> float
  {
    if (auto val = node.value<double>())
      return static_cast<float>(*val);
    return 0.0f;
  };

  return {getVal((*arr)[0]), getVal((*arr)[1])};
}

auto ConfigLoader::load() -> AnvlkConfig
{
  tomlparser::TOMLParser parser(m_configPath, m_logCtx);
  AnvlkConfig            cfg;

  // Font
  if (auto val = parser.get<TOMLKey>("font", "name"))
    cfg.font.name = TOMLKey{*val};
  if (auto val = parser.get<TOMLKey>("font", "path"))
    cfg.font.path = TOMLKey{*val};

  // Background
  if (auto val = parser.get<TOMLKey>("bg", "name"))
    cfg.bg.name = *val;
  if (auto val = parser.get<TOMLKey>("bg", "path"))
    cfg.bg.path = TOMLKey{*val};

  // Debug
  if (auto val = parser.get<TOMLKey>("debug", "debug_log_enable"))
    cfg.debug.debug_log_enable = (*val == "true" || *val == "1");

  // Time
  if (auto val = parser.get<TOMLKey>("time", "time_format"))
    cfg.time.time_format = TOMLKey{*val};

  // Time Box
  for (types::iters i = 0; i < 4; ++i)
  {
    if (auto arr = parser.getArray("time_box", TIMEBOXPOS[i]))
    {
      auto pos                 = parseVec2(*arr);
      cfg.timeBoxVertices[i].x = pos[0];
      cfg.timeBoxVertices[i].y = pos[1];
      cfg.timeBoxVertices[i].u = TEXCOORDS[i][0];
      cfg.timeBoxVertices[i].v = TEXCOORDS[i][1];
    }
  }

  LOG::INFO(m_logCtx, logger::LogStyle::COLOR_BOLD, "Configuration loaded successfully from '{}'",
            m_configPath.c_str());

  return cfg;
}

} // namespace anvlk::cfg
