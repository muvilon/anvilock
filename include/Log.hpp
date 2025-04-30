#pragma once

#include <anvilock/include/Types.hpp>
#include <anvilock/include/term/AnsiSchema.hpp>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace anvlk::logger
{

using namespace anvlk::term::ansi;

// Log levels (for filtering)
enum class LogLevel
{
  Trace,
  Debug,
  Info,
  Warn,
  Error,
  Critical
};

// ANSI color codes for log levels
inline auto logLevelColor(LogLevel level) -> const char*
{

  switch (level)
  {
    case LogLevel::Trace:
      return ansiWhite;
    case LogLevel::Debug:
      return ansiCyan;
    case LogLevel::Info:
      return ansiGreen;
    case LogLevel::Warn:
      return ansiYellow;
    case LogLevel::Error:
      return ansiRed;
    case LogLevel::Critical:
      return ansiBoldRed;
    default:
      return ansiReset;
  }
}

inline auto findLogLevelString(LogLevel level) -> const char*
{
  switch (level)
  {
    case LogLevel::Trace:
      return "TRACE";
    case LogLevel::Debug:
      return "DEBUG";
    case LogLevel::Info:
      return "INFO";
    case LogLevel::Warn:
      return "WARN";
    case LogLevel::Error:
      return "ERROR";
    case LogLevel::Critical:
      return "CRITICAL";
    default:
      return "UNKNOWN";
  }
}

// Log context struct to handle various settings
struct LogContext
{
  bool          toFile = false;
  types::fsPath logFilePath;
  bool          timestamp   = true;
  LogLevel      minLogLevel = LogLevel::Trace;
};

// Mutex for thread synchronization
inline std::mutex logMutex;

// Internal logger function (protected by mutex for thread-safety)
inline void logMessage(LogLevel level, const LogContext& context, const types::LogString& message)
{
  if (level < context.minLogLevel)
    return;

  std::lock_guard<std::mutex> lock(logMutex); // Locking for thread safety

  // Build timestamp string
  std::string timestamp_str;
  if (context.timestamp)
  {
    auto    now  = std::chrono::system_clock::now();
    auto    time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_r(&time, &tm);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    timestamp_str = oss.str();
  }

  // Log level string
  const char* level_str = findLogLevelString(level);

  // Console output
  types::LogString console_output;
  if (context.timestamp)
    console_output += std::format("[{}{}{}] ", ansiBold, timestamp_str, ansiReset);
  console_output += std::format("[{}{}{}] ", logLevelColor(level), level_str, ansiReset);
  console_output += message;

  std::cout << console_output << std::endl;

  // File output (plain, no ANSI)
  if (context.toFile)
  {
    std::ofstream log_file(context.logFilePath, std::ios_base::app);
    if (log_file)
    {
      types::LogString file_line;
      if (context.timestamp)
        file_line += std::format("[{}] ", timestamp_str);
      file_line += std::format("[{}] {}", level_str, message);
      log_file << file_line << std::endl;
    }
  }
}

// Variadic log formatting function
template <typename... Args>
inline void log(LogLevel level, const LogContext& ctx, std::format_string<Args...> fmt,
                Args&&... args)
{
  types::LogString message = std::format(fmt, std::forward<Args>(args)...);
  logMessage(level, ctx, std::move(message));
}

inline void init(const LogContext& context)
{
  std::ostringstream init_msg;

  init_msg << ansiBold << "ANVILOCK Logger initialized!" << ansiReset << "\n";

  if (context.toFile)
    init_msg << "  • Output: file logging to '" << context.logFilePath << "'\n";
  else
    init_msg << "  • Output: console only\n";

  init_msg << "  • Minimum level: " << logLevelColor(context.minLogLevel)
           << findLogLevelString(context.minLogLevel) << ansiReset << "\n";

  init_msg << "  • Timestamps: " << (context.timestamp ? "enabled" : "disabled");

  logMessage(LogLevel::Info, context, init_msg.str());
}

} // namespace anvlk::logger
