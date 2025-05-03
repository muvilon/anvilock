#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>

namespace anvlk::types
{

enum ExitCodes
{
  ANVLK_SUCCESS   = 0,
  ANVLK_UNDEFINED = -1,
  ANVLK_FAILED    = 1,
};

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8  = uint8_t;

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

using LogString         = std::string;
using LogStatus         = const char*;
using LogCategoryString = std::string;

using AuthString = std::string;
using PamString  = const char*;

using iter   = int;
using iters  = std::size_t; // size_t is the size of something in memory
using Status = int;

using Dimensions = i32;

using VPtr = void*;

using Coords = float;

using AnsiColor = const char*;

template <std::size_t N> using FloatArray = std::array<float, N>;
template <std::size_t N> using IntArray   = std::array<int, N>;
template <std::size_t N> using CStrArray  = std::array<const char*, N>;

} // namespace anvlk::types
