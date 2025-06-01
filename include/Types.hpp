#ifndef ANVLK_GLOBAL_TYPES_HPP
#define ANVLK_GLOBAL_TYPES_HPP

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace anvlk::types
{

//@--> [ Time-related aliases ] //
using SteadyClock = std::chrono::steady_clock; //@--> [ Monotonic clock for time measurements ]
using TimePoint   = SteadyClock::time_point;   //@--> [ Point in time using SteadyClock ]

//@--> [ Enum for program exit codes ] //
enum ExitCodes
{
  ANVLK_SUCCESS   = 0,  //@--> [ Logical status to be successful ] //
  ANVLK_UNDEFINED = -1, //@--> [ Unspecified or unknown failure ] //
  ANVLK_FAILED    = 1,  //@--> [ Explicit failure condition ] //
};

//@--> [ Enum for EGL return codes ] //
enum EGLCodes
{
  RET_CODE_FAIL    = 0, //@--> [ EGL call failed ] //
  RET_CODE_SUCCESS = 1  //@--> [ EGL call succeeded ] //
};

//@--> [ Unsigned integer aliases ] //
using u64 = uint64_t; //@--> [ 64-bit unsigned integer ] //
using u32 = uint32_t; //@--> [ 32-bit unsigned integer ] //
using u16 = uint16_t; //@--> [ 16-bit unsigned integer ] //
using u8  = uint8_t;  //@--> [ 8-bit unsigned integer ] //

using uint = unsigned int; //@--> [ Platform-defined unsigned int ] //

//@--> [ Signed integer aliases ] //
using i64 = int64_t; //@--> [ 64-bit signed integer ] //
using i32 = int32_t; //@--> [ 32-bit signed integer ] //
using i16 = int16_t; //@--> [ 16-bit signed integer ] //
using i8  = int8_t;  //@--> [ 8-bit signed integer ] //

//@--> [ Filesystem path aliases ] //
using fsPath = std::filesystem::path; //@--> [ Platform-native filesystem path ] //

//@--> [ Configuration key/value aliases ] //
using TOMLKey   = std::string; //@--> [ Key used in TOML config ] //
using TOMLTable = std::string; //@--> [ Table identifier in TOML config ] //
using AuthFlag  = bool;        //@--> [ Boolean auth control flag ] //

//@--> [ Filesystem and directory-related aliases ] //
using Path      = std::string; //@--> [ Generic filesystem path as string ] //
using PathCStr  = const char*; //@--> [ C-string version of a path ] //
using FileName  = const char*; //@--> [ File name as C-string ] //
using Directory = std::string; //@--> [ Directory path as string ] //

//@--> [ Shader-related aliases ] //
using ShaderName        = std::string; //@--> [ Name identifier for a shader ] //
using ShaderContent     = std::string; //@--> [ Shader source code as string ] //
using ShaderContentCStr = const char*; //@--> [ Shader source as C-string ] //

//@--> [ Image and texture data ] //
using ImageData = unsigned char; //@--> [ Raw byte for image pixel data ] //

//@--> [ Logging-related aliases ] //
using LogString         = std::string; //@--> [ Logging message text ] //
using LogStatus         = const char*; //@--> [ C-string status label for logging ] //
using LogCategoryString = std::string; //@--> [ Category tag for logging systems ] //

//@--> [ Authentication string aliases ] //
using AuthString = std::string; //@--> [ PAM or other auth string ] //
using PamString  = const char*; //@--> [ PAM message C-string ] //

//@--> [ Time string formatting ] //
using TimeString = std::string; //@--> [ Human-readable formatted time ] //

//@--> [ Loop counters and size indicators ] //
using iter   = int;         //@--> [ General-purpose loop counter ] //
using iters  = std::size_t; //@--> [ Memory-safe size/count type ] //
using Status = int;         //@--> [ Generic status code or state enum ] //

//@--> [ Rendering and UI dimensions ] //
using Dimensions = int; //@--> [ Width, height, etc. for UI/rendering ] //

//@--> [ Void pointer alias for generic raw data ] //
using VPtr = void*; //@--> [ Raw void pointer ] //

//@--> [ Coordinate values for layout or rendering ] //
using Coords = float; //@--> [ Floating-point coordinate value ] //

//@--> [ ANSI terminal color codes ] //
using AnsiColor = const char*; //@--> [ ANSI color escape code string ] //

//@--> [ Error messages and debug info ] //
using RuntimeErrorMessage = std::string_view; //@--> [ Lightweight error message view ] //
using RuntimeErrorString  = std::string;      //@--> [ Dynamic error message container ] //

//@--> [ Conversion helpers for type safety ] //
template <typename T> constexpr auto to_usize(T value) -> iters
{
  return static_cast<iters>(value); //@--> [ Convert to std::size_t alias ] //
}

template <typename T> constexpr auto to_float(T value) -> float
{
  return static_cast<float>(value); //@--> [ Convert to float ] //
}

template <typename T> constexpr auto to_double(T value) -> double
{
  return static_cast<double>(value); //@--> [ Convert to double ] //
}

//@--> [ Array templates for common data layouts ] //
template <std::size_t N> using FloatArray = std::array<float, N>; //@--> [ 1D float array ] //
template <std::size_t ROWS, std::size_t COLS>
using FloatArray2D = std::array<std::array<float, COLS>, ROWS>; //@--> [ 2D float array ] //
template <std::size_t N> using IntArray = std::array<int, N>;   //@--> [ 1D int array ] //
template <std::size_t N>
using CStrArray = std::array<const char*, N>;                   //@--> [ Array of C-strings ] //
template <std::size_t N> using CharArray = std::array<char, N>; //@--> [ Char buffer array ] //

} // namespace anvlk::types

#endif
