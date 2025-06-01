#ifndef ANVLK_CONFIG_HANDLER_HPP
#define ANVLK_CONFIG_HANDLER_HPP

#include <anvilock/ClientState.hpp>
#include <anvilock/Log.hpp>
#include <anvilock/Types.hpp>
#include <anvilock/config/ConfigStruct.hpp>
#include <anvilock/toml/Parser.hpp>

namespace anvlk::cfg
{
inline constexpr types::PathCStr REL_CFG_PATH  = ".config/anvilock/";
inline constexpr types::FileName CFG_FILE_NAME = "config.toml";
using TOMLKey                                  = anvlk::types::TOMLKey;

inline constexpr types::FloatArray2D<4, 2> TEXCOORDS = {{
  {0.0f, 0.0f}, // Top left
  {1.0f, 0.0f}, // Top right
  {0.0f, 1.0f}, // Bottom left
  {1.0f, 1.0f}  // Bottom right
}};

inline constexpr std::array<TOMLKey, 4> TIMEBOXPOS = {"top_left", "top_right", "bottom_left",
                                                      "bottom_right"};

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

#endif
