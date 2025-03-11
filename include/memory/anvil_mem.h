#ifndef ANVIL_MEM_H
#define ANVIL_MEM_H

// Secure memory zeroing
#define ANVIL_MEMZERO(ptr, size)                                \
  do                                                            \
  {                                                             \
    volatile unsigned char* p = (volatile unsigned char*)(ptr); \
    for (size_t i = 0; i < (size); i++)                         \
    {                                                           \
      p[i] = 0;                                                 \
    }                                                           \
  } while (0)

// Safe memory allocation macros
#define ANVIL_SAFE_ALLOC(ptr, type, count)                       \
  do                                                             \
  {                                                              \
    ptr = (type*)malloc((count) * sizeof(type));                 \
    if (!(ptr))                                                  \
    {                                                            \
      log_message(LOG_LEVEL_ERROR, "Memory allocation failed!"); \
      exit(EXIT_FAILURE);                                        \
    }                                                            \
  } while (0)

#define ANVIL_SAFE_CALLOC(ptr, type, count)                      \
  do                                                             \
  {                                                              \
    ptr = (type*)calloc((count), sizeof(type));                  \
    if (!(ptr))                                                  \
    {                                                            \
      log_message(LOG_LEVEL_ERROR, "Memory allocation failed!"); \
      exit(EXIT_FAILURE);                                        \
    }                                                            \
  } while (0)

// Safe memory reallocation
#define ANVIL_SAFE_REALLOC(ptr, type, new_count)                   \
  do                                                               \
  {                                                                \
    void* temp_ptr = realloc((ptr), (new_count) * sizeof(type));   \
    if (!temp_ptr)                                                 \
    {                                                              \
      log_message(LOG_LEVEL_ERROR, "Memory reallocation failed!"); \
      exit(EXIT_FAILURE);                                          \
    }                                                              \
    (ptr) = (type*)temp_ptr;                                       \
  } while (0)

// Safe freeing
#define ANVIL_SAFE_FREE(ptr) \
  do                         \
  {                          \
    if (ptr)                 \
    {                        \
      free(ptr);             \
      ptr = NULL;            \
    }                        \
  } while (0)

#endif
