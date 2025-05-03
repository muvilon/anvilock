#pragma once

#include <anvilock/include/Types.hpp>

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

// Top-level config
struct AnvlkConfig
{
  Font       font;
  Background bg;
  Debug      debug;
  Time       time;
  BoxCorners time_box;
};
