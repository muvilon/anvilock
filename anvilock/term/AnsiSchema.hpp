#ifndef ANVLK_TERM_ANSI_SCHEMA_HPP
#define ANVLK_TERM_ANSI_SCHEMA_HPP

#include <anvilock/Types.hpp>
#include <format>

namespace anvlk::term::ansi
{

#define DEFINE_ANSI_COLOR(name, code) inline constexpr types::AnsiColor name = code

// Reset & Effects
DEFINE_ANSI_COLOR(ansiReset, "\033[0m");
DEFINE_ANSI_COLOR(ansiBold, "\033[1m");
DEFINE_ANSI_COLOR(ansiUnderline, "\033[4m");
DEFINE_ANSI_COLOR(ansiBoldUnderline, "\033[1;4m");

// Standard Colors
DEFINE_ANSI_COLOR(ansiBlack, "\033[30m");
DEFINE_ANSI_COLOR(ansiRed, "\033[31m");
DEFINE_ANSI_COLOR(ansiGreen, "\033[32m");
DEFINE_ANSI_COLOR(ansiYellow, "\033[33m");
DEFINE_ANSI_COLOR(ansiBlue, "\033[34m");
DEFINE_ANSI_COLOR(ansiPurple, "\033[35m"); // Magenta
DEFINE_ANSI_COLOR(ansiCyan, "\033[36m");
DEFINE_ANSI_COLOR(ansiWhite, "\033[37m");

// Bold / Bright Colors
DEFINE_ANSI_COLOR(ansiBoldBlack, "\033[1;30m");
DEFINE_ANSI_COLOR(ansiBoldRed, "\033[1;31m");
DEFINE_ANSI_COLOR(ansiBoldGreen, "\033[1;32m");
DEFINE_ANSI_COLOR(ansiBoldYellow, "\033[1;33m");
DEFINE_ANSI_COLOR(ansiBoldBlue, "\033[1;34m");
DEFINE_ANSI_COLOR(ansiBoldPurple, "\033[1;35m"); // Magenta
DEFINE_ANSI_COLOR(ansiBoldCyan, "\033[1;36m");
DEFINE_ANSI_COLOR(ansiBoldWhite, "\033[1;37m");

// Extended 256-color (examples)
DEFINE_ANSI_COLOR(ansiGray, "\033[38;5;245m");
DEFINE_ANSI_COLOR(ansiBrightWhite, "\033[38;5;15m");
DEFINE_ANSI_COLOR(ansiOrange, "\033[38;5;208m");
DEFINE_ANSI_COLOR(ansiBoldOrange, "\033[1;38;5;208m");

inline auto bold(const types::LogString& text) -> types::LogString
{
  return std::format("{}{}{}", ansiBold, text, ansiReset);
}

inline auto boldLog(const types::LogString& text) -> types::LogString
{
  return std::format("[{}{}{}]", ansiBold, text, ansiReset);
}

inline auto boldUnderline(const types::LogString& text) -> types::LogString
{
  return std::format("{}{}{}", ansiBoldUnderline, text, ansiReset);
}

inline auto underline(const types::LogString& text) -> types::LogString
{
  return std::format("{}{}{}", ansiUnderline, text, ansiReset);
}

inline auto color(const char* colorCode, const types::LogString& text) -> types::LogString
{
  return std::format("{}{}{}", colorCode, text, ansiReset);
}

} // namespace anvlk::term::ansi

#endif
