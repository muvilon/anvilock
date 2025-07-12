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
  TOMLKey                     time_format; // Prefer std::chrono formatting if applicable
  anvlk::types::FloatArray<4> shadowColor = {0.0f, 0.0f, 0.0f, 0.2f};
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

struct PasswordFieldConfig
{
  anvlk::types::FloatArray<4> shadowColor = {0.0f, 0.0f, 0.0f, 0.25f}; // default RGBA
  double                      width       = 0.6;
  double                      height      = 0.12;
};

// Top-level config
struct AnvlkConfig
{
  Font                  font;
  Background            bg;
  Debug                 debug;
  Time                  time;
  std::array<Vertex, 4> timeBoxVertices;

  PasswordFieldConfig pwdFieldCfg;
};

#endif
