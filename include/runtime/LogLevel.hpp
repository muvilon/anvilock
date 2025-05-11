#ifndef ANVLK_RUNTIME_LOG_LEVEL_HPP
#define ANVLK_RUNTIME_LOG_LEVEL_HPP

#include <algorithm>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <cstdlib>

namespace anvlk::runtime::logger
{
inline auto getLogLevelFromEnv() -> anvlk::logger::LogLevel
{
  types::LogStatus logLevelCStr = std::getenv("ANVLK_LOG_LEVEL");

  if (!logLevelCStr)
    return anvlk::logger::LogLevel::Debug;

  std::string logLevelStr{logLevelCStr};
  // ensure that the string is always in uppercase
  std::ranges::transform(logLevelStr, logLevelStr.begin(), ::toupper);

  if (logLevelStr == "TRACE")
    return anvlk::logger::LogLevel::Trace;
  else if (logLevelStr == "INFO")
    return anvlk::logger::LogLevel::Info;
  else if (logLevelStr == "WARN")
    return anvlk::logger::LogLevel::Warn;
  else if (logLevelStr == "ERROR")
    return anvlk::logger::LogLevel::Error;
  else if (logLevelStr == "DEBUG")
    return anvlk::logger::LogLevel::Debug;
  else
    return anvlk::logger::LogLevel::Debug; // fallback
}
} // namespace anvlk::runtime::logger

#endif
