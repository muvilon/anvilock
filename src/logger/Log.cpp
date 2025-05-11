#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/term/AnsiSchema.hpp>
#include <anvilock/include/term/OutputStream.hpp>
#include <fstream>
#include <iostream>
#include <utility>

namespace anvlk::logger
{

template void log<>(LogLevel, const LogContext&, std::format_string<>);
template void log<>(LogLevel, const LogContext&, LogStyle, std::format_string<>);

inline constexpr int MAX_LEVEL_STRING_LENGTH = 5;

inline constexpr auto logLevelColor(LogLevel level) -> types::AnsiColor
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
    default:
      return term::ansi::ansiReset;
  }
}

inline constexpr auto makeCategory(types::AnsiColor color, types::LogCategoryString name)
  -> std::pair<types::AnsiColor, types::LogCategoryString>
{
  return std::make_pair(color, name);
}

inline constexpr auto findLogCategory(LogCategory category)
  -> std::pair<types::AnsiColor, types::LogCategoryString>
{
  using namespace anvlk::term::ansi;

  switch (category)
  {
    case anvlk::logger::LogCategory::MAIN:
      return makeCategory(ansiBold, "MAIN");
    case anvlk::logger::LogCategory::CONFIG:
      return makeCategory(ansiBoldRed, "CONFIG");
    case anvlk::logger::LogCategory::TOML:
      return makeCategory(ansiBoldPurple, "TOML");
    case anvlk::logger::LogCategory::EGL:
      return makeCategory(ansiBoldOrange, "EGL");
    case anvlk::logger::LogCategory::PAM:
      return makeCategory(ansiBoldCyan, "PAM");
    case anvlk::logger::LogCategory::SHM:
      return makeCategory(ansiBoldGreen, "SHM");
    case anvlk::logger::LogCategory::WL_KB:
      return makeCategory(ansiBoldBlue, "WL_KB");
    case anvlk::logger::LogCategory::WL_PTR:
      return makeCategory(ansiBoldBlue, "WL_PTR");
    case anvlk::logger::LogCategory::WL_OUT:
      return makeCategory(ansiBoldBlue, "WL_OUT");
    case anvlk::logger::LogCategory::WL_SEAT:
      return makeCategory(ansiBoldBlue, "WL_SEAT");
    case anvlk::logger::LogCategory::WL_REG:
      return makeCategory(ansiBoldBlue, "WL_REG");
    case anvlk::logger::LogCategory::SESSION_LOCK:
      return makeCategory(ansiBoldBlue, "SESSION_LOCK");
    case anvlk::logger::LogCategory::XDG_SURFACE:
      return makeCategory(ansiBoldYellow, "XDG_SURFACE");
    case anvlk::logger::LogCategory::XDG_WMBASE:
      return makeCategory(ansiBoldYellow, "XDG_WMBASE");
    case anvlk::logger::LogCategory::SHADERS:
      return makeCategory(ansiGray, "SHADERS");
    default:
      return makeCategory(ansiBold, "MAIN");
  }
}

inline constexpr auto findLogLevelString(LogLevel level) -> std::pair<types::LogStatus, int>
{
  switch (level)
  {
    case LogLevel::Trace:
      return std::make_pair("TRACE", 5); // Log level, len(log level)
    case LogLevel::Debug:
      return std::make_pair("DEBUG", 5);
    case LogLevel::Info:
      return std::make_pair("INFO", 4);
    case LogLevel::Warn:
      return std::make_pair("WARN", 4);
    case LogLevel::Error:
      return std::make_pair("ERROR", 5);
    default:
      return std::make_pair("UNKNW", 5);
  }
}

inline constexpr auto styleLogMessage(const LogLevel level, const types::LogString& logMsg,
                                      LogStyle style) -> term::ostream::StyledText
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
  auto                     levelStrPair     = findLogLevelString(level);
  auto                     logCategoryPair  = findLogCategory(context.category);
  types::LogCategoryString logCategoryStr   = logCategoryPair.second;
  types::AnsiColor         logCategoryColor = logCategoryPair.first;
  const auto paddedSpaces = term::ostream::padSpaces(MAX_LEVEL_STRING_LENGTH - levelStrPair.second);

  // Console output
  types::LogString console_output;
  if (context.timestamp)
    console_output += term::ansi::boldLog(timestamp_str);
  console_output += std::format(" [{}{}{}]{} ", logLevelColor(level), levelStrPair.first,
                                term::ansi::ansiReset, paddedSpaces);

  console_output +=
    std::format(" [{}{}{}] ", logCategoryColor, logCategoryStr, term::ansi::ansiReset);

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
      file_line +=
        std::format("[{}]{} [{}] {}", levelStrPair.first, paddedSpaces, logCategoryStr, message);
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

  log(LogLevel::Info, context, LogStyle::BOLD, "  • Minimum level: {}",
      term::ansi::color(logLevelColor(context.minLogLevel),
                        findLogLevelString(context.minLogLevel).first));

  log(LogLevel::Info, context, LogStyle::BOLD, "  • Timestamps: {}",
      (context.timestamp ? term::ansi::color(term::ansi::ansiBoldGreen, "ENABLED")
                         : term::ansi::color(term::ansi::ansiBoldOrange, "DISABLED")));
}

void LogContext::changeContext(LogCategory newCategory)
{
  std::scoped_lock lock(anvlk::logger::logMutex);
  category = newCategory;
}

void LogContext::resetContext()
{
  std::scoped_lock lock(anvlk::logger::logMutex);
  category = LogCategory::MAIN;
}

void switchCtx(LogContext& ctx, LogCategory category) { ctx.changeContext(category); }

void resetCtx(LogContext& ctx) { ctx.resetContext(); }

} // namespace anvlk::logger
