#ifndef LOG_H
#define LOG_H

#include "client_state.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Define color codes */
#define COLOR_RESET    "\x1B[0m"
#define COLOR_RED      "\x1B[1;31m"
#define COLOR_YELLOW   "\x1B[1;33m"
#define COLOR_BLUE     "\x1B[1;34m"
#define COLOR_DARKGRAY "\x1B[1;30m"
#define COLOR_MAGENTA  "\x1B[1;35m"
#define COLOR_GREEN    "\x1B[1;32m"
#define COLOR_CYAN     "\x1B[1;36m"
#define COLOR_WHITE    "\x1B[1;37m"
#define COLOR_LIGHTRED "\x1B[1;91m"

/* Define log importance levels */
enum log_importance
{
  LOG_LEVEL_SILENT,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARN,
  LOG_LEVEL_INFO,
  LOG_LEVEL_AUTH,
  LOG_LEVEL_SUCCESS,
  LOG_LEVEL_TRACE, // New level for detailed tracing
  LOG_LEVEL_ALERT, // New level for alerts
  LOG_LEVEL_DEBUG,
  LOG_IMPORTANCE_LAST // Keep this as the last element to define the range
};

/* Current log importance level */
static enum log_importance log_importance = LOG_LEVEL_DEBUG;
static char*               debug_option   = NULL;
static bool                debug_on       = false;

/* Define verbosity colors using macros */
static const char* verbosity_colors[] = {
  [LOG_LEVEL_SILENT]  = "",             // No color for silent
  [LOG_LEVEL_ERROR]   = COLOR_RED,      // Red for errors
  [LOG_LEVEL_WARN]    = COLOR_YELLOW,   // Yellow for warnings
  [LOG_LEVEL_INFO]    = COLOR_BLUE,     // Blue for information
  [LOG_LEVEL_AUTH]    = COLOR_MAGENTA,  // Magenta for authentication
  [LOG_LEVEL_SUCCESS] = COLOR_GREEN,    // Green for success
  [LOG_LEVEL_DEBUG]   = COLOR_WHITE,    // Dark gray for debugging
  [LOG_LEVEL_TRACE]   = COLOR_CYAN,     // Cyan for tracing
  [LOG_LEVEL_ALERT]   = COLOR_LIGHTRED, // Light red for alerts
};

/* Function to initialize logging with a specified verbosity level */
static void init_debug(struct client_state* state)
{
  if (!debug_option)
  {
    debug_option = strdup(state->user_configs.debug_log_option);
  }

  if (debug_option != NULL && strcmp(debug_option, "true") == 0)
  {
    fprintf(stderr, "%s", "[LOG] DEBUG LOGS ENABLED\n");
    debug_on = true;
  }
}

/* Logging function */
void log_message(enum log_importance verbosity, const char* fmt, ...)
{
  if (verbosity > log_importance || (verbosity == LOG_LEVEL_DEBUG && !debug_on))
  {
    return;
  }

  va_list args;
  va_start(args, fmt);

  // Prefix the time to the log message
  time_t     t       = time(NULL);
  struct tm* tm_info = localtime(&t);
  char       buffer[26];
  strftime(buffer, sizeof(buffer), "[%F %T] - ", tm_info);
  fprintf(stderr, "%s", buffer);

  unsigned c = (verbosity < LOG_IMPORTANCE_LAST) ? verbosity : LOG_IMPORTANCE_LAST - 1;

  if (isatty(STDERR_FILENO))
  {
    fprintf(stderr, "%s", verbosity_colors[c]);
  }

  vfprintf(stderr, fmt, args);

  if (isatty(STDERR_FILENO))
  {
    fprintf(stderr, COLOR_RESET); // Reset color after message
  }

  fprintf(stderr, "\n");
  va_end(args);
}

/* Optional function to strip leading './' from file paths */
const char* strip_path(const char* filepath)
{
  while (*filepath == '.' || *filepath == '/')
  {
    ++filepath;
  }
  return filepath;
}

#endif // LOG_H
