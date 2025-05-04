#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/config/ConfigStruct.hpp>
#include <anvilock/include/toml/Parser.hpp>

namespace anvlk::cfg
{
inline constexpr types::PathCStr REL_CFG_PATH  = ".config/anvilock/";
inline constexpr types::FileName CFG_FILE_NAME = "config.toml";
using TOMLKey                                  = anvlk::types::TOMLKey;

class ConfigLoader
{
public:
  // Constructor with optional custom path
  explicit ConfigLoader(anvlk::logger::LogContext& logCtx, types::Directory& homeDir,
                        const types::fsPath& customPath = {});

  // Load configuration from file
  auto load() -> AnvlkConfig;

  // Get the configuration path
  [[nodiscard]] auto getConfigPath() const -> types::fsPath;

private:
  anvlk::logger::LogContext& m_logCtx;
  types::PathCStr            m_homeDir;
  types::fsPath              m_configPath;

  // Helper method to parse float arrays for box corners
  static auto parseVec2(const toml::array* arr) -> anvlk::types::FloatArray<2>;

  // Get default config path
  auto getUserConfigPath() -> types::fsPath;
};

} // namespace anvlk::cfg
