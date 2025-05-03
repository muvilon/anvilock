#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/toml/Parser.hpp>

auto main() -> int
{
  anvlk::logger::LogContext logCtx;

  logCtx = {.logFilePath = "log.txt"};

  anvlk::logger::init(logCtx);
  anvlk::tomlparser::TOMLParser reader("example.toml", logCtx);

  std::cout << "\n-> Top-level and nested tables:\n";
  for (const auto& t : reader.listTables())
    std::cout << "  [" << t << "]\n";

  std::cout << "\n-> Top-level keys:\n";
  for (const auto& k : reader.listKeys("network"))
    std::cout << "  " << k << "\n";

  std::optional<std::string> netIP = reader.get<anvlk::types::TOMLKey>("network", "ip");

  std::cout << "\n==> network.ip = " << *netIP << std::endl;

  std::cout << "\n-> All values (recursive):\n";
  reader.dumpAll();

  return anvlk::types::ANVLK_SUCCESS;
}
