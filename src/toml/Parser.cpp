#include <anvilock/include/toml/Parser.hpp>

namespace anvlk::tomlparser
{

TOMLParser::TOMLParser(const anvlk::types::fsPath& path, anvlk::logger::LogContext& logCtx)
{
  try
  {
    m_data = toml::parse_file(path.string());
  }
  catch (const toml::parse_error& err)
  {
    const auto&            src      = err.source();
    const types::LogString location = std::format("{}:{}", src.begin.line, src.begin.column);
    anvlk::logger::log(anvlk::logger::LogLevel::Error, logCtx, "Parse error: {} at {}",
                       err.description(), location);
    exit(EXIT_FAILURE);
  }
}

auto TOMLParser::contains(const anvlk::types::TOMLKey& key) const -> bool
{
  return m_data.contains(key);
}

auto TOMLParser::contains(const anvlk::types::TOMLTable& table,
                          const anvlk::types::TOMLKey&   key) const -> bool
{
  if (auto tbl = m_data[table].as_table())
    return tbl->contains(key);
  return false;
}

auto TOMLParser::listTables() const -> std::vector<anvlk::types::TOMLTable>
{
  std::vector<std::string> results;
  list_tables_recursive(m_data, "", results);
  return results;
}

auto TOMLParser::listKeys(const types::TOMLTable& table) const -> std::vector<types::TOMLKey>
{
  std::vector<types::TOMLKey> keys;
  if (table.empty())
  {
    for (const auto& [k, v] : m_data)
    {
      if (!v.is_table())
        keys.emplace_back(k.str());
    }
  }
  else if (auto tbl = m_data[table].as_table())
  {
    for (const auto& [k, v] : *tbl)
    {
      if (!v.is_table())
        keys.emplace_back(k.str());
    }
  }
  return keys;
}

void TOMLParser::dumpAll() const { dumpRecursive(m_data, ""); }

void TOMLParser::list_tables_recursive(const toml::table& tbl, const std::string& prefix,
                                       std::vector<std::string>& out) const
{
  for (const auto& [k, v] : tbl)
  {
    if (v.is_table())
    {
      auto                  key_str  = types::TOMLKey(k.str());
      anvlk::types::TOMLKey full_key = prefix.empty() ? key_str : prefix + "." + key_str;
      out.push_back(full_key);
      list_tables_recursive(*v.as_table(), full_key, out);
    }
  }
}

void TOMLParser::printNode(std::ostream& os, const toml::node& v) const
{
  if (v.is_string())
    os << "\"" << *v.as_string() << "\"";
  else if (v.is_integer())
    os << v.value_or(0);
  else if (v.is_floating_point())
    os << v.value_or(0.0);
  else if (v.is_boolean())
    os << std::boolalpha << v.value_or(false);
  else if (v.is_array())
    os << "[array]";
  else if (v.is_table())
    os << "{table}";
  else
    os << "[unknown]";

  os << "\n";
}

void TOMLParser::dumpRecursive(const toml::table& tbl, const std::string& prefix) const
{
  for (const auto& [k, v] : tbl)
  {
    auto                  key_str  = types::TOMLKey(k.str());
    anvlk::types::TOMLKey full_key = prefix.empty() ? key_str : prefix + "." + key_str;
    if (v.is_table())
    {
      std::cout << "[" << full_key << "]\n";
      dumpRecursive(*v.as_table(), full_key);
    }
    else
    {
      printNode(std::cout << full_key << " = ", v);
    }
  }
}

auto TOMLParser::getArray(const types::TOMLTable& table, const types::TOMLKey& key) const
  -> std::optional<const toml::array*>
{
  if (auto tbl = m_data[table].as_table())
    if (auto arr = (*tbl)[key].as_array())
      return arr;
  return std::nullopt;
}

} // namespace anvlk::tomlparser
