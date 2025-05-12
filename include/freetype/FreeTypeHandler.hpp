#ifndef ANVLK_FREETYPE_HANDLER_HPP
#define ANVLK_FREETYPE_HANDLER_HPP

#include <anvilock/include/LogMacros.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/freetype/FreeTypeStruct.hpp>
#include <stdexcept>
#include <utility>

namespace anvlk::freetype
{

template <typename T> constexpr auto to_ft_ulong(T value) -> FT_ULong
{
  return static_cast<FT_ULong>(value);
}

using namespace anvlk;

constexpr int DOT_RADIUS     = 6;
constexpr int CHAR_HEIGHT_PX = 20;
constexpr int CHAR_WIDTH_PX  = 10;

class FreeTypeHandler
{
public:
  explicit FreeTypeHandler(logger::LogContext& ctx, const anvlk::types::Path& fontPath)
      : m_logCtx(ctx), m_fontPath(std::move(fontPath))
  {
    using style = logger::LogStyle;

    logger::switchCtx(ctx, logger::LogCategory::FREETYPE);

    if (FT_Init_FreeType(&m_ftLibrary))
    {
      LOG::ERROR(m_logCtx, style::COLOR_BOLD, "Failed to initialize FreeType");
      throw std::runtime_error("FreeType init failed");
    }

    if (FT_New_Face(m_ftLibrary, m_fontPath.c_str(), 0, &m_ftFace))
    {
      LOG::ERROR(m_logCtx, style::COLOR_BOLD, "Failed to load font: '{}'", m_fontPath);
      throw std::runtime_error("Font load failed");
    }

    if (FT_Set_Pixel_Sizes(m_ftFace, 0, CHAR_HEIGHT_PX))
    {
      LOG::ERROR(m_logCtx, style::COLOR_BOLD, "Failed to set font size to {}px", CHAR_HEIGHT_PX);
      throw std::runtime_error("Set font size failed");
    }

    LOG::INFO(m_logCtx, "FreeType2 initialized => font loaded successfully");

    logger::resetCtx(ctx);
  }

  ~FreeTypeHandler()
  {
    if (m_ftFace)
      FT_Done_Face(m_ftFace);
    if (m_ftLibrary)
      FT_Done_FreeType(m_ftLibrary);
  }

  [[nodiscard]] auto face() const -> FT_Face { return m_ftFace; }
  [[nodiscard]] auto library() const -> FT_Library { return m_ftLibrary; }

private:
  logger::LogContext& m_logCtx;
  anvlk::types::Path  m_fontPath;
  FT_Library          m_ftLibrary{nullptr};
  FT_Face             m_ftFace{nullptr};
};

} // namespace anvlk::freetype

#endif
