#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <wayland-client.h>

namespace anvlk::wl
{

using logL = logger::LogLevel;

enum PointerEventMask : u32
{
  Enter        = 1 << 0,
  Leave        = 1 << 1,
  Motion       = 1 << 2,
  Button       = 1 << 3,
  Axis         = 1 << 4,
  AxisSource   = 1 << 5,
  AxisStop     = 1 << 6,
  AxisDiscrete = 1 << 7,
};

void onPointerEnter(types::VPtr data, types::wayland::WLPointer_*, u32 serial,
                    types::wayland::WLSurface_*, types::wayland::WLFixed_ x,
                    types::wayland::WLFixed_ y);
void onPointerLeave(types::VPtr data, types::wayland::WLPointer_*, u32 serial,
                    types::wayland::WLSurface_*);
void onPointerMotion(types::VPtr data, types::wayland::WLPointer_*, u32 time,
                     types::wayland::WLFixed_ x, types::wayland::WLFixed_ y);
void onPointerButton(types::VPtr data, types::wayland::WLPointer_*, u32 serial, u32 time,
                     u32 button, u32 state);
void onPointerAxis(types::VPtr data, types::wayland::WLPointer_*, u32 time, u32 axis,
                   types::wayland::WLFixed_ value);
void onPointerAxisSource(types::VPtr data, types::wayland::WLPointer_*, u32 source);
void onPointerAxisStop(types::VPtr data, types::wayland::WLPointer_*, u32 time, u32 axis);
void onPointerAxisDiscrete(types::VPtr data, types::wayland::WLPointer_*, u32 axis, i32 discrete);
void onPointerFrame(types::VPtr data, types::wayland::WLPointer_*);

inline constexpr wl_pointer_listener kPointerListener{
  .enter                   = onPointerEnter,
  .leave                   = onPointerLeave,
  .motion                  = onPointerMotion,
  .button                  = onPointerButton,
  .axis                    = onPointerAxis,
  .frame                   = onPointerFrame,
  .axis_source             = onPointerAxisSource,
  .axis_stop               = onPointerAxisStop,
  .axis_discrete           = onPointerAxisDiscrete,
  .axis_value120           = nullptr,
  .axis_relative_direction = nullptr,
};

} // namespace anvlk::wl
