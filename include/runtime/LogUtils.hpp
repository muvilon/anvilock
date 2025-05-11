#ifndef ANVLK_RUNTIME_LOG_UTILS_HPP
#define ANVLK_RUNTIME_LOG_UTILS_HPP

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

inline auto generateLogFilePath(types::PathCStr home) -> types::Path
{
  // Get HOME directory
  if (!home)
    throw std::runtime_error("HOME environment variable is not set");

  const std::filesystem::path logDir = std::filesystem::path(home) / ".cache" / "anvilock";

  // Create directory if it doesn't exist
  std::filesystem::create_directories(logDir);

  // Get current time
  auto    now  = std::chrono::system_clock::now();
  auto    time = std::chrono::system_clock::to_time_t(now);
  std::tm tm{};
  localtime_r(&time, &tm);

  // Format file name: log-%d-%H-%S.log
  std::ostringstream filename;
  filename << "log-" << std::put_time(&tm, "%d-%m--%H-%M-%S") << ".log";

  return (logDir / filename.str()).string();
}

} // namespace anvlk::runtime::logger

#endif
