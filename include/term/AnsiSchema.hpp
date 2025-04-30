#pragma once

#include <anvilock/include/Types.hpp>
#include <format>

namespace anvlk::term::ansi
{

#define DEFINE_ANSI_COLOR(name, code) inline constexpr types::AnsiColor name = code

DEFINE_ANSI_COLOR(ansiBold, "\033[1m");
DEFINE_ANSI_COLOR(ansiUnderline, "\033[4m");
DEFINE_ANSI_COLOR(ansiBoldUnderline, "\033[1;4m");
DEFINE_ANSI_COLOR(ansiReset, "\033[0m");
DEFINE_ANSI_COLOR(ansiWhite, "\033[37m");
DEFINE_ANSI_COLOR(ansiCyan, "\033[36m");
DEFINE_ANSI_COLOR(ansiGreen, "\033[32m");
DEFINE_ANSI_COLOR(ansiYellow, "\033[33m");
DEFINE_ANSI_COLOR(ansiRed, "\033[31m");
DEFINE_ANSI_COLOR(ansiBoldRed, "\033[1;31m");
DEFINE_ANSI_COLOR(ansiBoldGreen, "\033[1;32m");
DEFINE_ANSI_COLOR(ansiBoldYellow, "\033[1;33m");
DEFINE_ANSI_COLOR(ansiBoldBlue, "\033[1;34m");

inline auto bold(const types::LogString& text) -> types::LogString
{
  return std::format("{}{}{}", ansiBold, text, ansiReset);
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
