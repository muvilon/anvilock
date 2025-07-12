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

template <typename T> auto GetTomlValue(const toml::node& node, T fallback = T{}) -> T
{
  static_assert(std::is_arithmetic_v<T>, "get_toml_value requires an arithmetic type");

  if constexpr (std::is_floating_point_v<T>)
  {
    if (const auto* val_f = node.as_floating_point())
      return static_cast<T>(val_f->get());
    if (const auto* val_i = node.as_integer())
      return static_cast<T>(val_i->get());
  }
  else if constexpr (std::is_integral_v<T>)
  {
    if (const auto* val_i = node.as_integer())
      return static_cast<T>(val_i->get());
    if (const auto* val_f = node.as_floating_point())
      return static_cast<T>(val_f->get());
  }

  return fallback;
}

auto ConfigLoader::getConfigPath() const -> types::fsPath { return m_configPath; }

template <std::size_t N>
auto ConfigLoader::parseVecN(const toml::array* arr) -> anvlk::types::FloatArray<N>
{
  anvlk::types::FloatArray<N> result{};
  if (!arr || arr->size() != N)
    return result;

  for (types::iters i = 0; i < N; ++i)
    result[i] = GetTomlValue<float>((*arr)[i]);

  return result;
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

  if (auto arr = parser.getArray("time_box", "shadow_color"))
  {
    if (arr->get().size() == 4)
      cfg.time.shadowColor = parseVecN<4>(&arr->get());
  }

  // Time Box
  for (types::iters i = 0; i < 4; ++i)
  {
    if (auto arr = parser.getArray("time_box", TIMEBOXPOS[i]))
    {
      auto pos                 = parseVecN<2>(&arr->get());
      cfg.timeBoxVertices[i].x = pos[0];
      cfg.timeBoxVertices[i].y = pos[1];
      cfg.timeBoxVertices[i].u = TEXCOORDS[i][0];
      cfg.timeBoxVertices[i].v = TEXCOORDS[i][1];
    }
  }

  // Password Field Shadow Color
  if (auto arr = parser.getArray("password_field", "shadow_color"))
  {
    if (arr->get().size() == 4)
      cfg.pwdFieldCfg.shadowColor = parseVecN<4>(&arr->get());
  }

  if (auto pwdFieldWidth = parser.get<double>("password_field", "width"))
  {
    cfg.pwdFieldCfg.width = *pwdFieldWidth;
  }

  if (auto pwdFieldHeight = parser.get<double>("password_field", "height"))
  {
    cfg.pwdFieldCfg.height = *pwdFieldHeight;
  }

  LOG::INFO(m_logCtx, logger::LogStyle::COLOR_BOLD, "Configuration loaded successfully from '{}'",
            m_configPath.c_str());

  return cfg;
}

} // namespace anvlk::cfg
