#ifndef ANVLK_TERM_OUTPUT_STREAM_HPP
#define ANVLK_TERM_OUTPUT_STREAM_HPP

#include <anvilock/include/Types.hpp>
#include <anvilock/include/term/AnsiSchema.hpp>
#include <ostream>
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

inline constexpr auto build_style_prefix(std::initializer_list<types::AnsiColor> styles)
  -> std::string
{
  std::string result;
  for (const auto& style : styles)
  {
    result += style;
  }
  return result;
}

inline constexpr auto styleStream(std::string_view                        text,
                                  std::initializer_list<types::AnsiColor> styles) -> StyledText
{
  return {
    .stylePrefix = build_style_prefix(styles), .text = text, .styleSuffix = term::ansi::ansiReset};
}

inline constexpr auto boldStream(std::string_view text) -> StyledText
{
  return styleStream(text, {term::ansi::ansiBold});
}

inline constexpr auto underlineStream(std::string_view text) -> StyledText
{
  return styleStream(text, {term::ansi::ansiUnderline});
}

inline constexpr auto colorStream(std::string_view text, types::AnsiColor color) -> StyledText
{
  return styleStream(text, {color});
}

inline constexpr auto boldColorStream(std::string_view text, types::AnsiColor color) -> StyledText
{
  return styleStream(text, {term::ansi::ansiBold, color});
}

inline constexpr auto underlineColorStream(std::string_view text, types::AnsiColor color)
  -> StyledText
{
  return styleStream(text, {term::ansi::ansiUnderline, color});
}

inline constexpr auto padSpaces(int length) -> std::string { return std::string(length, ' '); }

} // namespace anvlk::term::ostream

#endif
