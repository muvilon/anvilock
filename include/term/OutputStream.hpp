#pragma once

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

  friend auto operator<<(std::ostream& os, const StyledText& styled) -> std::ostream&;
};

// Combine multiple ANSI codes into a single escape sequence
auto build_style_prefix(std::initializer_list<types::AnsiColor> styles) -> std::string;

// Generic style combinator
auto styleStream(std::string_view text, std::initializer_list<types::AnsiColor> styles)
  -> StyledText;

// Shortcuts
auto boldStream(std::string_view text) -> StyledText;
auto underlineStream(std::string_view text) -> StyledText;
auto colorStream(std::string_view text, types::AnsiColor color) -> StyledText;
auto boldColorStream(std::string_view text, types::AnsiColor color) -> StyledText;
auto underlineColorStream(std::string_view text, types::AnsiColor color) -> StyledText;

} // namespace anvlk::term::ostream
