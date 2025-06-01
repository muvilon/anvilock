#ifndef ANVLK_TOML_PARSER_HPP
#define ANVLK_TOML_PARSER_HPP

#include <anvilock/Log.hpp>
#include <anvilock/Types.hpp>
#include <external/toml++/toml.hpp>
#include <iostream>
#include <optional>
#include <string>

namespace anvlk::tomlparser
{

class TOMLParser
{
public:
  explicit TOMLParser(const anvlk::types::fsPath& path, anvlk::logger::LogContext& logCtx);

  template <typename T> auto get(const anvlk::types::TOMLKey& key) const -> std::optional<T>;

  template <typename T>
  auto get(const anvlk::types::TOMLTable& table, const anvlk::types::TOMLKey& key) const
    -> std::optional<T>;

  [[nodiscard]] auto contains(const anvlk::types::TOMLKey& key) const -> bool;
  [[nodiscard]] auto contains(const anvlk::types::TOMLTable& table,
                              const anvlk::types::TOMLKey&   key) const -> bool;

  [[nodiscard]] auto listTables() const -> std::vector<anvlk::types::TOMLTable>;
  [[nodiscard]] auto listKeys(const types::TOMLTable& table) const -> std::vector<types::TOMLKey>;

  void dumpAll() const;

  [[nodiscard]] auto getArray(const types::TOMLTable& table, const types::TOMLKey& key) const
    -> std::optional<const toml::array*>;

private:
  anvlk::logger::LogContext m_logCtx;
  toml::table               m_data;

  void list_tables_recursive(const toml::table& tbl, const std::string& prefix,
                             std::vector<std::string>& out) const;

  void printNode(std::ostream& os, const toml::node& v) const;

  void dumpRecursive(const toml::table& tbl, const std::string& prefix) const;
};

} // namespace anvlk::tomlparser

// Template definitions must stay in the header
namespace anvlk::tomlparser
{
template <typename T>
auto TOMLParser::get(const anvlk::types::TOMLKey& key) const -> std::optional<T>
{
  if (!m_data.is_table())
    return std::nullopt;
  if (auto val = m_data[key].value<T>())
    return val;
  return std::nullopt;
}

template <typename T>
auto TOMLParser::get(const anvlk::types::TOMLTable& table, const anvlk::types::TOMLKey& key) const
  -> std::optional<T>
{
  if (auto tbl = m_data[table].as_table())
  {
    if (auto val = (*tbl)[key].value<T>())
      return val;
  }
  return std::nullopt;
}
} // namespace anvlk::tomlparser

#endif
