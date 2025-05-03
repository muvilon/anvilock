#include <anvilock/include/Log.hpp>
#include <fstream>

namespace anvlk::logger
{

template void log<>(LogLevel, const LogContext&, std::format_string<>);
template void log<>(LogLevel, const LogContext&, LogStyle, std::format_string<>);

using logL = anvlk::logger::LogLevel;

inline auto logLevelColor(LogLevel level) -> types::AnsiColor
{
  switch (level)
  {
    case LogLevel::Trace:
      return term::ansi::ansiBoldPurple;
    case LogLevel::Debug:
      return term::ansi::ansiBoldCyan;
    case LogLevel::Info:
      return term::ansi::ansiBoldGreen;
    case LogLevel::Warn:
      return term::ansi::ansiBoldYellow;
    case LogLevel::Error:
      return term::ansi::ansiBoldRed;
    case LogLevel::Critical:
      return term::ansi::ansiBoldRed;
    default:
      return term::ansi::ansiReset;
  }
}

inline auto findLogLevelString(LogLevel level) -> types::LogStatus
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

inline auto styleLogMessage(const LogLevel level, const types::LogString& logMsg, LogStyle style)
  -> term::ostream::StyledText
{
  using namespace term;
  const auto color = logLevelColor(level);

  switch (style)
  {
    case LogStyle::BOLD:
      return ostream::boldStream(logMsg);
    case LogStyle::UNDERLINE:
      return ostream::underlineStream(logMsg);
    case LogStyle::COLOR:
      return ostream::colorStream(logMsg, color);
    case LogStyle::COLOR_BOLD:
      return ostream::boldColorStream(logMsg, color);
    case LogStyle::COLOR_UNDERLINE:
      return ostream::underlineColorStream(logMsg, color);
    case LogStyle::COLOR_BOLD_UNDERLINE:
      return ostream::styleStream(logMsg, {ansi::ansiBold, ansi::ansiUnderline, color});
    case LogStyle::NONE:
    default:
      return {.stylePrefix = "", .text = logMsg, .styleSuffix = ""};
  }
}

inline void logMessage(LogLevel level, const LogContext& context, const types::LogString& message,
                       LogStyle style)
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
  types::LogStatus levelStr = findLogLevelString(level);

  // Console output
  types::LogString console_output;
  if (context.timestamp)
    console_output += term::ansi::boldLog(timestamp_str);
  console_output +=
    std::format(" [{}{}{}] ", logLevelColor(level), levelStr, term::ansi::ansiReset);

  std::cout << console_output << styleLogMessage(level, message, style) << std::endl;

  // File output (plain, no ANSI)
  if (context.toFile)
  {
    std::ofstream log_file(context.logFilePath, std::ios_base::app);
    if (log_file)
    {
      types::LogString file_line;
      if (context.timestamp)
        file_line += std::format("[{}] ", timestamp_str);
      file_line += std::format("[{}] {}", levelStr, message);
      log_file << file_line << std::endl;
    }
  }
}

void init(const LogContext& context)
{
  log(LogLevel::Info, context, LogStyle::COLOR_BOLD, "ANVILOCK Logger initialized!!");

  if (context.toFile)
    log(LogLevel::Info, context, LogStyle::BOLD, "  • Output: file logging to '{}'",
        term::ansi::color(term::ansi::ansiBoldPurple, context.logFilePath.c_str()));
  else
    log(LogLevel::Info, context, LogStyle::BOLD, "  • Output: console only");

  log(
    LogLevel::Info, context, LogStyle::BOLD, "  • Minimum level: {}",
    term::ansi::color(logLevelColor(context.minLogLevel), findLogLevelString(context.minLogLevel)));

  log(LogLevel::Info, context, LogStyle::BOLD, "  • Timestamps: {}",
      (context.timestamp ? term::ansi::color(term::ansi::ansiBoldGreen, "ENABLED")
                         : term::ansi::color(term::ansi::ansiBoldOrange, "DISABLED")));
}

} // namespace anvlk::logger
