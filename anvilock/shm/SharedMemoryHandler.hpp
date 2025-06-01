#ifndef ANVLK_SHM_SHARED_MEMORY_HANDLER_HPP
#define ANVLK_SHM_SHARED_MEMORY_HANDLER_HPP

#include <anvilock/Types.hpp>
#include <chrono>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

namespace anvlk::types::shm
{
using SHMFileName = std::string;
}

namespace anvlk::shm
{

class SharedMemoryHandler
{
public:
  static auto allocate(size_t size) -> int
  {
    int fd = createUniqueShmFile();
    if (fd < 0)
      return -1;

    while (ftruncate(fd, size) < 0 && errno == EINTR)
    {
      // Retry on interruption
    }

    if (errno != 0)
    {
      close(fd);
      return -1;
    }

    return fd;
  }

private:
  static auto generateName() -> std::string
  {
    auto now  = std::chrono::high_resolution_clock::now().time_since_epoch();
    long nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    types::shm::SHMFileName name = "/wl_shm-";
    for (int i = 0; i < 6; ++i)
    {
      char c = 'A' + (nsec & 15) + ((nsec & 16) ? 16 : 0);
      name += c;
      nsec >>= 5;
    }
    return name;
  }

  static auto createUniqueShmFile() -> int
  {
    constexpr int kMaxRetries = 100;
    for (int i = 0; i < kMaxRetries; ++i)
    {
      types::shm::SHMFileName name = generateName();
      int                     fd   = shm_open(name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0600);
      if (fd >= 0)
      {
        shm_unlink(name.c_str()); // Immediately unlink; anonymous after fd is opened
        return fd;
      }
      else if (errno != EEXIST)
      {
        break;
      }
    }
    return -1;
  }
};

} // namespace anvlk::shm

#endif
