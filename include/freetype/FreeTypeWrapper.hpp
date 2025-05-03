#pragma once

#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/config/ConfigHandler.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdexcept>
#include <utility>

namespace freetype
{

using namespace anvlk;

constexpr int DOT_RADIUS  = 6;
constexpr int CHAR_HEIGHT = 20;
constexpr int CHAR_WIDTH  = 10;

class FreeTypeWrapper
{
public:
  explicit FreeTypeWrapper(logger::LogContext ctx, const anvlk::types::Path& fontPath)
      : m_logCtx{std::move(ctx)}, m_fontPath(std::move(fontPath))
  {
    using logL = logger::LogLevel;
    using logS = logger::LogStyle;

    if (FT_Init_FreeType(&m_ftLibrary))
    {
      logger::log(logL::Error, m_logCtx, logS::COLOR_BOLD, "Failed to initialize FreeType");
      throw std::runtime_error("FreeType init failed");
    }

    if (FT_New_Face(m_ftLibrary, m_fontPath, 0, &m_ftFace))
    {
      logger::log(logL::Error, m_logCtx, logS::COLOR_BOLD, "Failed to load font: '{}'", m_fontPath);
      throw std::runtime_error("Font load failed");
    }

    if (FT_Set_Pixel_Sizes(m_ftFace, 0, CHAR_HEIGHT))
    {
      logger::log(logL::Error, m_logCtx, logS::COLOR_BOLD, "Failed to set font size to {}px",
                  CHAR_HEIGHT);
      throw std::runtime_error("Set font size failed");
    }

    logger::log(logL::Info, m_logCtx, "FreeType2 initialized => font loaded successfully");
  }

  ~FreeTypeWrapper()
  {
    if (m_ftFace)
      FT_Done_Face(m_ftFace);
    if (m_ftLibrary)
      FT_Done_FreeType(m_ftLibrary);
  }

  [[nodiscard]] auto face() const -> FT_Face { return m_ftFace; }
  [[nodiscard]] auto library() const -> FT_Library { return m_ftLibrary; }

private:
  FT_Library         m_ftLibrary{nullptr};
  FT_Face            m_ftFace{nullptr};
  anvlk::types::Path m_fontPath;
  logger::LogContext m_logCtx;
};

} // namespace freetype
