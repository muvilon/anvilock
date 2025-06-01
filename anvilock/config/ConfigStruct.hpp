#ifndef ANVLK_CONFIG_STRUCT_HPP
#define ANVLK_CONFIG_STRUCT_HPP

#include <anvilock/Types.hpp>

using namespace anvlk::types;

struct Font
{
  TOMLKey name;
  TOMLKey path;
};

struct Background
{
  TOMLKey name;
  TOMLKey path;
};

struct Debug
{
  bool debug_log_enable = false;
};

struct Time
{
  TOMLKey time_format; // Prefer std::chrono formatting if applicable
};

struct BoxCorners
{
  anvlk::types::FloatArray<2> top_left;
  anvlk::types::FloatArray<2> top_right;
  anvlk::types::FloatArray<2> bottom_left;
  anvlk::types::FloatArray<2> bottom_right;
};

struct Vertex
{
  Coords x{}, y{}, u{}, v{};
};

// Top-level config
struct AnvlkConfig
{
  Font                  font;
  Background            bg;
  Debug                 debug;
  Time                  time;
  std::array<Vertex, 4> timeBoxVertices;
};

#endif
