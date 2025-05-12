#ifndef ANVLK_PWDBUF_PASSWORD_BUFFER_HPP
#define ANVLK_PWDBUF_PASSWORD_BUFFER_HPP

#include <anvilock/include/Types.hpp>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

namespace anvlk::pwdbuf
{
static bool     mlock_supported = true;
static long int page_size       = 0;

// Retrieve page size
inline auto get_page_size() -> long int
{
  if (!page_size)
  {
    page_size = sysconf(_SC_PAGESIZE);
  }
  return page_size;
}

// Function to lock a password buffer into memory
inline auto password_buffer_lock(types::VPtr addr, size_t size) -> bool
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
          return false;
        }
        break;
      case EPERM:
        mlock_supported = false;
        return true;
      default:
        return false;
    }
  }
  return true;
}

// Function to unlock a password buffer from memory
inline auto password_buffer_unlock(types::VPtr addr, size_t size) -> bool
{
  if (mlock_supported)
  {
    if (munlock(addr, size) != 0)
    {
      return false;
    }
  }
  return true;
}

// Class to manage a secure password buffer
class PasswordBuffer
{
public:
  PasswordBuffer(size_t size)
  {
    // Allocate buffer with proper alignment
    if (posix_memalign(&m_buffer, types::to_usize(get_page_size()), size) != 0)
    {
      throw std::bad_alloc();
    }

    // Lock the buffer in memory
    if (!password_buffer_lock(m_buffer, size))
    {
      throw std::runtime_error("Failed to lock password buffer");
    }
    m_size = size;
  }

  // Destructor to clear and free the buffer securely
  ~PasswordBuffer()
  {
    clear();
    password_buffer_unlock(m_buffer, m_size);
    free(m_buffer);
  }

  // Accessor for the buffer
  auto get() -> types::VPtr { return m_buffer; }

  // Clear the buffer securely
  void clear()
  {
    if (m_buffer)
    {
      std::memset(m_buffer, 0, m_size);
    }
  }

private:
  types::VPtr m_buffer = nullptr;
  size_t      m_size   = 0;
};
} // namespace anvlk::pwdbuf

#endif
