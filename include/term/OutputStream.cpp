#include <anvilock/include/term/OutputStream.hpp>

namespace anvlk::term::ostream
{

auto operator<<(std::ostream& os, const StyledText& styled) -> std::ostream&
{
  return os << styled.stylePrefix << styled.text << styled.styleSuffix;
}

auto build_style_prefix(std::initializer_list<types::AnsiColor> styles) -> std::string
{
  std::string result;
  for (const auto& style : styles)
  {
    result += style;
  }
  return result;
}

auto styleStream(std::string_view text, std::initializer_list<types::AnsiColor> styles)
  -> StyledText
{
  return {
    .stylePrefix = build_style_prefix(styles), .text = text, .styleSuffix = term::ansi::ansiReset};
}

auto boldStream(std::string_view text) -> StyledText
{
  return styleStream(text, {term::ansi::ansiBold});
}

auto underlineStream(std::string_view text) -> StyledText
{
  return styleStream(text, {term::ansi::ansiUnderline});
}

auto colorStream(std::string_view text, types::AnsiColor color) -> StyledText
{
  return styleStream(text, {color});
}

auto boldColorStream(std::string_view text, types::AnsiColor color) -> StyledText
{
  return styleStream(text, {term::ansi::ansiBold, color});
}

auto underlineColorStream(std::string_view text, types::AnsiColor color) -> StyledText
{
  return styleStream(text, {term::ansi::ansiUnderline, color});
}

} // namespace anvlk::term::ostream
