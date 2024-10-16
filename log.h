#ifndef LOG_H
#define LOG_H

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Define log importance levels */
enum log_importance
{
    LOG_LEVEL_SILENT,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_AUTH,
    LOG_LEVEL_SUCCESS,
    LOG_LEVEL_DEBUG,
    LOG_IMPORTANCE_LAST // Keep this as the last element to define the range
};

/* Current log importance level */
static enum log_importance log_importance = LOG_LEVEL_DEBUG;

/* Define verbosity colors */
static const char* verbosity_colors[] = {
    [LOG_LEVEL_SILENT]  = "",
    [LOG_LEVEL_ERROR]   = "\x1B[1;31m", // Red
    [LOG_LEVEL_WARN]    = "\x1B[1;33m", // Yellow
    [LOG_LEVEL_INFO]    = "\x1B[1;34m", // Blue
    [LOG_LEVEL_DEBUG]   = "\x1B[1;30m", // Dark Gray
    [LOG_LEVEL_AUTH]    = "\x1B[1;35m", // Pink (Magenta)
    [LOG_LEVEL_SUCCESS] = "\x1B[1;32m"  // Green
};

/* Log file pointer */
static FILE* log_file = NULL;

/* Function to initialize logging with a specified verbosity level */
void log_init(enum log_importance verbosity)
{
    if (verbosity < LOG_IMPORTANCE_LAST)
    {
        log_importance = verbosity;
    }

    // Open the log file for writing (this will overwrite the existing file)
    log_file = fopen("casuality.log", "w");
    if (log_file == NULL)
    {
        fprintf(stderr, "Failed to open log file: %s\n", strerror(errno));
        return; // Exit early if log file cannot be opened
    }

    // Disable buffering for the log file
    setvbuf(log_file, NULL, _IONBF, 0);
}

/* Logging function */
void log_message(enum log_importance verbosity, const char* fmt, ...)
{
    if (verbosity > log_importance)
    {
        return;
    }

    va_list args;
    va_start(args, fmt);

    // Prefix the time to the log message
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char buffer[26];
    strftime(buffer, sizeof(buffer), "[%F %T] - ", tm_info);
    
    // Log to console
    fprintf(stderr, "%s", buffer);

    unsigned c = (verbosity < LOG_IMPORTANCE_LAST) ? verbosity : LOG_IMPORTANCE_LAST - 1;

    if (isatty(STDERR_FILENO))
    {
        fprintf(stderr, "%s", verbosity_colors[c]);
    }

    vfprintf(stderr, fmt, args);

    if (isatty(STDERR_FILENO))
    {
        fprintf(stderr, "\x1B[0m"); // Reset color
    }

    fprintf(stderr, "\n");

    // Also log to the file
    if (log_file != NULL)
    {
        fprintf(log_file, "%s", buffer);
        vfprintf(log_file, fmt, args);
        fprintf(log_file, "\n");
        fflush(log_file); // Flush to ensure it is written immediately
    }

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

/* Function to clean up logging resources */
void log_cleanup()
{
    if (log_file != NULL)
    {
        fclose(log_file);
        log_file = NULL;
    }
}

#endif // LOG_H
