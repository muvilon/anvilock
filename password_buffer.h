#ifndef PWD_BUF_H
#define PWD_BUF_H

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static bool     mlock_supported = true;
static long int page_size       = 0;

static long int get_page_size()
{
  if (!page_size)
  {
    page_size = sysconf(_SC_PAGESIZE);
  }
  return page_size;
}

// Password buffer lock expects addr to be page aligned
static bool password_buffer_lock(char* addr, size_t size)
{
  int retries = 5;
  while (mlock(addr, size) != 0 && retries > 0)
  {
    switch (errno)
    {
      case EAGAIN:
        retries--;
        if (retries == 0)
        {
          log_message(LOG_LEVEL_ERROR, "mlock() supported but failed too often.");
          return false;
        }
        break;
      case EPERM:
        log_message(LOG_LEVEL_ERROR, "Unable to mlock() password memory: Unsupported!");
        mlock_supported = false;
        return true;
      default:
        log_message(LOG_LEVEL_ERROR, "Unable to mlock() password memory.");
        return false;
    }
  }
  return true;
}

// Password buffer unlock expects addr to be page aligned
static bool password_buffer_unlock(char* addr, size_t size)
{
  if (mlock_supported)
  {
    if (munlock(addr, size) != 0)
    {
      log_message(LOG_LEVEL_ERROR, "Unable to munlock() password memory.");
      return false;
    }
  }
  return true;
}

// Create a secure password buffer
char* password_buffer_create(size_t size)
{
  void* buffer;
  int   result = posix_memalign(&buffer, get_page_size(), size);
  if (result)
  {
    errno = result; // posix_memalign doesn't set errno according to the man page
    log_message(LOG_LEVEL_ERROR, "Failed to allocate password buffer");
    return NULL;
  }

  if (!password_buffer_lock(buffer, size))
  {
    free(buffer);
    return NULL;
  }

  return (char*)buffer;
}

// Clear the buffer securely
void clear_buffer(char* buffer, size_t size)
{
  if (buffer)
  {
    memset(buffer, 0, size); // Clear the buffer
  }
}

// Destroy the password buffer securely
void password_buffer_destroy(char* buffer, size_t size)
{
  if (buffer)
  {
    clear_buffer(buffer, size);
    password_buffer_unlock(buffer, size);
    free(buffer);
  }
}

#endif
