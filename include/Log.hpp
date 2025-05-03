#pragma once

#include <anvilock/include/Types.hpp>
#include <anvilock/include/term/AnsiSchema.hpp>
#include <anvilock/include/term/OutputStream.hpp>
#include <mutex>
#include <iostream>

namespace anvlk::logger
{

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

// Log style enums for styling logs
enum LogStyle
{
  NONE,
  BOLD,
  UNDERLINE,
  COLOR,
  COLOR_BOLD,
  COLOR_UNDERLINE,
  COLOR_BOLD_UNDERLINE,
};

// Log context struct to handle various settings
struct LogContext
{
  bool          toFile = false;
  types::fsPath logFilePath;
  bool          timestamp   = true;
  LogLevel      minLogLevel = LogLevel::Trace;
};

inline std::mutex    logMutex;

// Function prototypes
void logMessage(LogLevel level, const LogContext& context, const types::LogString& message,
                LogStyle style = LogStyle::NONE);

template <typename... Args>
inline void log(LogLevel level, const LogContext& ctx, const LogStyle logStyle,
                std::format_string<Args...> fmt, Args&&... args)
{
  types::LogString message = std::format(fmt, std::forward<Args>(args)...);
  logMessage(level, ctx, std::move(message), logStyle);
}

template <typename... Args>
inline void log(LogLevel level, const LogContext& ctx, std::format_string<Args...> fmt,
                Args&&... args)
{
  log(level, ctx, LogStyle::NONE, fmt, std::forward<Args>(args)...);
}

void init(const LogContext& context);

} // namespace anvlk::logger
