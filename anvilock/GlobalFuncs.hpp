#ifndef ANVLK_GLOBAL_GLOBAL_FUNCS_HPP
#define ANVLK_GLOBAL_GLOBAL_FUNCS_HPP

#include "Types.hpp"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <pwd.h>
#include <string>
#include <string_view>
#include <unistd.h>
#include <utility>

namespace anvlk::utils
{

// Constants
inline constexpr types::Dimensions FallbackScreenWidth  = 1920;
inline constexpr types::Dimensions FallbackScreenHeight = 1080;
inline constexpr types::Directory  FallbackHomeDir      = "/root";

// Math utilities
template <typename T> [[nodiscard]] constexpr auto max(T a, T b) noexcept -> T
{
  return (a > b) ? a : b;
}

template <typename T> [[nodiscard]] constexpr auto min(T a, T b) noexcept -> T
{
  return (a < b) ? a : b;
}

template <typename T> [[nodiscard]] constexpr auto clamp(T value, T low, T high) noexcept -> T
{
  return min(max(value, low), high);
}

template <typename T> constexpr void swap(T& a, T& b) noexcept
{
  T temp = std::move(a);
  a      = std::move(b);
  b      = std::move(temp);
}

// Get HOME directory
[[nodiscard]] inline auto getHomeDir() -> anvlk::types::Directory
{
  const char* home = std::getenv("HOME");
  return home ? std::string(home) : FallbackHomeDir;
}

[[nodiscard]] inline auto getCurrentUsername() -> std::optional<types::AuthString>
{
  uid_t   uid = geteuid();     // Get effective user ID
  passwd* pw  = getpwuid(uid); // Get passwd struct for this UID
  if (pw && pw->pw_name)
  {
    return std::string(pw->pw_name);
  }
  return std::nullopt;
}

// Safe string concatenation
[[nodiscard]] inline auto safeStrJoin(std::string_view s1, std::string_view s2) -> std::string
{
  return std::string(s1) + std::string(s2);
}

// Get current time string in a safe format
inline auto getTimeString(std::string_view format) -> std::string
{
  using namespace std::chrono;

  auto    now   = system_clock::now();
  auto    now_c = system_clock::to_time_t(now);
  std::tm local_tm{};

  if (localtime_r(&now_c, &local_tm) == nullptr)
  {
    throw std::runtime_error("Failed to get local time");
  }

  std::ostringstream stream;
  stream << std::put_time(&local_tm, "%H:%M:%S"); // Default format
  if (format == "H:M" || format == "h:m")
  {
    stream.str(""); // Clear the current string
    stream << std::put_time(&local_tm, "%H:%M");
  }
  else if (format != "H:M:S" && format != "h:m:s")
  {
    stream.str(""); // Clear the current string
    stream << std::put_time(&local_tm, "%H:%M:%S");
  }

  return stream.str();
}

// Power-of-two utility (for GPU texture safety)
[[nodiscard]] constexpr auto nextPowerOfTwo(int v) noexcept -> int
{
  if (v <= 0)
    return 1;
  --v;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  return ++v;
}

// Vertex arrays (exposed as views)
inline constexpr anvlk::types::FloatArray<16> vertices_with_texcoords = {
  -0.27f, 0.9f, 0.0f, 0.0f, 0.27f, 0.9f, 1.0f, 0.0f,
  -0.27f, 0.7f, 0.0f, 1.0f, 0.27f, 0.7f, 1.0f, 1.0f};

inline constexpr anvlk::types::FloatArray<8> quad_vertices = {-1.0f, 1.0f, -1.0f, -1.0f,
                                                              1.0f,  1.0f, 1.0f,  -1.0f};

inline constexpr anvlk::types::FloatArray<6> triangle_vertices = {0.0f,  0.5f, -0.5f,
                                                                  -0.5f, 0.5f, -0.5f};

inline constexpr anvlk::types::FloatArray<8> tex_coords = {0.0f, 0.0f, 0.0f, 1.0f,
                                                           1.0f, 0.0f, 1.0f, 1.0f};

inline constexpr anvlk::types::FloatArray<8> password_field_vertices = {-0.4f, 0.1f, -0.4f, -0.1f,
                                                                        0.4f,  0.1f, 0.4f,  -0.1f};

inline constexpr anvlk::types::FloatArray<8> dot_vertices = {-0.015f, 0.015f, -0.015f, -0.015f,
                                                             0.015f,  0.015f, 0.015f,  -0.015f};

} // namespace anvlk::utils

#endif
