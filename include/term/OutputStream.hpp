#pragma once

#include <anvilock/include/Types.hpp>
#include <anvilock/include/term/AnsiSchema.hpp>
#include <string>
#include <string_view>

namespace anvlk::term::ostream
{

struct StyledText
{
  std::string      stylePrefix;
  std::string_view text;
  std::string_view styleSuffix;

  friend auto operator<<(std::ostream& os, const StyledText& styled) -> std::ostream&
  {
    return os << styled.stylePrefix << styled.text << styled.styleSuffix;
  }
};

// Combine multiple ANSI codes into a single escape sequence
inline auto build_style_prefix(std::initializer_list<types::AnsiColor> styles) -> std::string
{
  std::string result;
  for (const auto& style : styles)
  {
    result += style;
  }
  return result;
}

// Generic style combinator
inline auto styleStream(std::string_view text, std::initializer_list<types::AnsiColor> styles)
  -> StyledText
{
  return {
    .stylePrefix = build_style_prefix(styles), .text = text, .styleSuffix = term::ansi::ansiReset};
}

// Shortcuts
inline auto boldStream(std::string_view text) -> StyledText
{
  return styleStream(text, {term::ansi::ansiBold});
}

inline auto underlineStream(std::string_view text) -> StyledText
{
  return styleStream(text, {term::ansi::ansiUnderline});
}

inline auto colorStream(std::string_view text, types::AnsiColor color) -> StyledText
{
  return styleStream(text, {color});
}

inline auto boldColorStream(std::string_view text, types::AnsiColor color) -> StyledText
{
  return styleStream(text, {term::ansi::ansiBold, color});
}

inline auto underlineColorStream(std::string_view text, types::AnsiColor color) -> StyledText
{
  return styleStream(text, {term::ansi::ansiUnderline, color});
}

} // namespace anvlk::term::ostream
