#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace anvlk::types
{

using SteadyClock = std::chrono::steady_clock;
using TimePoint   = SteadyClock::time_point;

enum ExitCodes
{
  ANVLK_SUCCESS   = 0,
  ANVLK_UNDEFINED = -1,
  ANVLK_FAILED    = 1,
};

enum EGLCodes
{
  RET_CODE_FAIL    = 0,
  RET_CODE_SUCCESS = 1
};

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8  = uint8_t;

using uint = unsigned int;

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8  = int8_t;

using fsPath = std::filesystem::path;

using TOMLKey   = std::string;
using TOMLTable = std::string;
using AuthFlag  = bool;

using Path      = std::string;
using PathCStr  = const char*;
using FileName  = const char*;
using Directory = std::string;

using ShaderName        = std::string;
using ShaderContent     = std::string;
using ShaderContentCStr = const char*;

using ImageData = unsigned char;

using LogString         = std::string;
using LogStatus         = const char*;
using LogCategoryString = std::string;

using AuthString = std::string;
using PamString  = const char*;

using TimeString = std::string;

using iter   = int;
using iters  = std::size_t; // size_t is the size of something in memory
using Status = int;

using Dimensions = i32;

using VPtr = void*;

using Coords = float;

using AnsiColor = const char*;

using RuntimeErrorMessage = std::string_view;
using RuntimeErrorString  = std::string;

template <std::size_t N> using FloatArray = std::array<float, N>;
template <std::size_t ROWS, std::size_t COLS>
using FloatArray2D                       = std::array<std::array<float, COLS>, ROWS>;
template <std::size_t N> using IntArray  = std::array<int, N>;
template <std::size_t N> using CStrArray = std::array<const char*, N>;
template <std::size_t N> using CharArray = std::array<char, N>;

} // namespace anvlk::types
