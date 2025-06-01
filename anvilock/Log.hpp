#ifndef ANVLK_GLOBAL_LOG_HPP
#define ANVLK_GLOBAL_LOG_HPP

#include <anvilock/Types.hpp>
#include <anvilock/term/AnsiSchema.hpp>
#include <anvilock/term/OutputStream.hpp>
#include <mutex>

//////////////////////////////////////////////////
//
// !!!NEVER USE THIS FILE STANDALONE!!!
//
// Use it through LogMacros.hpp
//
/////////////////////////////////////////////////

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

enum LogCategory
{
  CONFIG,
  TOML,
  PAM,
  WL_REG,
  WL_KB,
  WL_PTR,
  WL_SEAT,
  WL_OUT,
  SESSION_LOCK,
  XDG_SURFACE,
  XDG_WMBASE,
  EGL,
  SHM,
  SHADERS,
  FREETYPE,
  MAIN
};

// Log context struct to handle various settings
struct LogContext
{
  bool          toFile = false;
  types::fsPath logFilePath;
  bool          timestamp   = true;
  LogLevel      minLogLevel = LogLevel::Trace;
  LogCategory   category    = LogCategory::MAIN;

public:
  void changeContext(LogCategory category);
  void resetContext();
};

inline std::mutex logMutex;

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
void switchCtx(LogContext& ctx, LogCategory category);
void resetCtx(LogContext& ctx);

} // namespace anvlk::logger

#endif
