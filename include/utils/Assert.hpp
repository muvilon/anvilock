#ifndef ANVLK_UTILS_ASSERT_HPP
#define ANVLK_UTILS_ASSERT_HPP

#include <cstdlib>
#include <iostream>
#include <source_location>

#define ANVLK_ASSERT(expr)                                                                \
  ((expr) ? static_cast<void>(0)                                                          \
          : (::anvlk::internal::assertionFailure(#expr, std::source_location::current()), \
             std::abort()))

// Comparison asserts
#define ANVLK_ASSERT_EQ(lhs, rhs) \
  (::anvlk::internal::assertEqual((lhs), (rhs), #lhs, #rhs, std::source_location::current()))

#define ANVLK_ASSERT_NE(lhs, rhs) \
  (::anvlk::internal::assertNotEqual((lhs), (rhs), #lhs, #rhs, std::source_location::current()))

#define ANVLK_ASSERT_GT(lhs, rhs) \
  (::anvlk::internal::assertGreater((lhs), (rhs), #lhs, #rhs, std::source_location::current()))

#define ANVLK_ASSERT_LT(lhs, rhs) \
  (::anvlk::internal::assertLess((lhs), (rhs), #lhs, #rhs, std::source_location::current()))

#define ANVLK_ASSERT_GE(lhs, rhs) \
  (::anvlk::internal::assertGreaterEqual((lhs), (rhs), #lhs, #rhs, std::source_location::current()))

#define ANVLK_ASSERT_LE(lhs, rhs) \
  (::anvlk::internal::assertLessEqual((lhs), (rhs), #lhs, #rhs, std::source_location::current()))

namespace anvlk::internal
{

inline void assertionFailure(const char*          expr,
                             std::source_location loc = std::source_location::current())
{
  std::cerr << "[ANVLK_ASSERT] Failed: " << expr << "\n"
            << "  at " << loc.file_name() << ":" << loc.line() << " (" << loc.function_name()
            << ")\n";
}

template <typename T, typename U>
inline void assertEqual(const T& lhs, const U& rhs, const char* lhs_str, const char* rhs_str,
                        std::source_location loc = std::source_location::current())
{
  if (!(lhs == rhs))
  {
    std::cerr << "[ANVLK_ASSERT_EQ] " << lhs_str << " != " << rhs_str << "\n"
              << "  Values: " << lhs << " != " << rhs << "\n"
              << "  at " << loc.file_name() << ":" << loc.line() << "\n";
    std::abort();
  }
}

template <typename T, typename U>
inline void assertNotEqual(const T& lhs, const U& rhs, const char* lhs_str, const char* rhs_str,
                           std::source_location loc = std::source_location::current())
{
  if (!(lhs != rhs))
  {
    std::cerr << "[ANVLK_ASSERT_NE] " << lhs_str << " == " << rhs_str << "\n"
              << "  Values: " << lhs << " == " << rhs << "\n"
              << "  at " << loc.file_name() << ":" << loc.line() << "\n";
    std::abort();
  }
}

template <typename T, typename U>
inline void assertGreater(const T& lhs, const U& rhs, const char* lhs_str, const char* rhs_str,
                          std::source_location loc = std::source_location::current())
{
  if (!(lhs > rhs))
  {
    std::cerr << "[ANVLK_ASSERT_GT] " << lhs_str << " <= " << rhs_str << "\n"
              << "  Values: " << lhs << " <= " << rhs << "\n"
              << "  at " << loc.file_name() << ":" << loc.line() << "\n";
    std::abort();
  }
}

template <typename T, typename U>
inline void assertLess(const T& lhs, const U& rhs, const char* lhs_str, const char* rhs_str,
                       std::source_location loc = std::source_location::current())
{
  if (!(lhs < rhs))
  {
    std::cerr << "[ANVLK_ASSERT_LT] " << lhs_str << " >= " << rhs_str << "\n"
              << "  Values: " << lhs << " >= " << rhs << "\n"
              << "  at " << loc.file_name() << ":" << loc.line() << "\n";
    std::abort();
  }
}

template <typename T, typename U>
inline void assertGreaterEqual(const T& lhs, const U& rhs, const char* lhs_str, const char* rhs_str,
                               std::source_location loc = std::source_location::current())
{
  if (!(lhs >= rhs))
  {
    std::cerr << "[ANVLK_ASSERT_GE] " << lhs_str << " < " << rhs_str << "\n"
              << "  Values: " << lhs << " < " << rhs << "\n"
              << "  at " << loc.file_name() << ":" << loc.line() << "\n";
    std::abort();
  }
}

template <typename T, typename U>
inline void assertLessEqual(const T& lhs, const U& rhs, const char* lhs_str, const char* rhs_str,
                            std::source_location loc = std::source_location::current())
{
  if (!(lhs <= rhs))
  {
    std::cerr << "[ANVLK_ASSERT_LE] " << lhs_str << " > " << rhs_str << "\n"
              << "  Values: " << lhs << " > " << rhs << "\n"
              << "  at " << loc.file_name() << ":" << loc.line() << "\n";
    std::abort();
  }
}

} // namespace anvlk::internal

#endif
