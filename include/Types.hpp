#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>

namespace anvlk::types
{

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8  = uint8_t;

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8  = int8_t;

using fsPath = std::filesystem::path;

using TOMLKey  = std::string;
using AuthFlag = bool;

using Path      = std::string;
using Directory = std::string;

using LogString = std::string;

using AuthString = std::string;
using PamString  = const char*;

using iter   = int;
using Status = int;

using Coords = float;

using AnsiColor = const char*;

template <std::size_t N> using FloatArray = std::array<float, N>;
template <std::size_t N> using IntArray   = std::array<int, N>;

} // namespace anvlk::types
