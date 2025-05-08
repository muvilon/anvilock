#pragma once

#include <anvilock/include/Log.hpp>

namespace LOG
{

#define DEFINE_LOG_NAMESPACE(NAME, LEVEL_ENUM)                                                   \
  template <typename... Args>                                                                    \
  inline void NAME(const anvlk::logger::LogContext& ctx, const anvlk::logger::LogStyle logStyle, \
                   std::format_string<Args...> fmt, Args&&... args)                              \
  {                                                                                              \
    anvlk::types::LogString message = std::format(fmt, std::forward<Args>(args)...);             \
    logMessage(anvlk::logger::LogLevel::LEVEL_ENUM, ctx, std::move(message), logStyle);          \
  }                                                                                              \
                                                                                                 \
  template <typename... Args>                                                                    \
  inline void NAME(const anvlk::logger::LogContext& ctx, std::format_string<Args...> fmt,        \
                   Args&&... args)                                                               \
  {                                                                                              \
    NAME(ctx, anvlk::logger::LogStyle::NONE, fmt, std::forward<Args>(args)...);                  \
  }

DEFINE_LOG_NAMESPACE(TRACE, Trace)
DEFINE_LOG_NAMESPACE(DEBUG, Debug)
DEFINE_LOG_NAMESPACE(INFO, Info)
DEFINE_LOG_NAMESPACE(WARN, Warn)
DEFINE_LOG_NAMESPACE(ERROR, Error)

} // namespace LOG
