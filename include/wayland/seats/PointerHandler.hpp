#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <cstring>
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

inline void onPointerEnter(void* data, wl_pointer*, u32 serial, wl_surface*, wl_fixed_t x,
                           wl_fixed_t y)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::Enter;
  ev.serial   = serial;
  ev.surfaceX = x;
  ev.surfaceY = y;
}

inline void onPointerLeave(void* data, wl_pointer*, u32 serial, wl_surface*)
{
  auto& cs = *static_cast<ClientState*>(data);
  cs.pointerEvent.eventMask |= PointerEventMask::Leave;
  cs.pointerEvent.serial = serial;
}

inline void onPointerMotion(void* data, wl_pointer*, u32 time, wl_fixed_t x, wl_fixed_t y)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::Motion;
  ev.time     = time;
  ev.surfaceX = x;
  ev.surfaceY = y;
}

inline void onPointerButton(void* data, wl_pointer*, u32 serial, u32 time, u32 button, u32 state)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::Button;
  ev.time   = time;
  ev.serial = serial;
  ev.button = button;
  ev.state  = state;
}

inline void onPointerAxis(void* data, wl_pointer*, u32 time, u32 axis, wl_fixed_t value)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::Axis;
  ev.time             = time;
  ev.axes[axis].valid = true;
  ev.axes[axis].value = value;
}

inline void onPointerAxisSource(void* data, wl_pointer*, u32 source)
{
  auto& cs = *static_cast<ClientState*>(data);
  cs.pointerEvent.eventMask |= PointerEventMask::AxisSource;
  cs.pointerEvent.axisSource = source;
}

inline void onPointerAxisStop(void* data, wl_pointer*, u32 time, u32 axis)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::AxisStop;
  ev.time             = time;
  ev.axes[axis].valid = true;
}

inline void onPointerAxisDiscrete(void* data, wl_pointer*, u32 axis, i32 discrete)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::AxisDiscrete;
  ev.axes[axis].valid    = true;
  ev.axes[axis].discrete = discrete;
}

inline void onPointerFrame(void* data, wl_pointer*)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  logger::log(logL::Debug, cs.logCtx, "Pointer frame @ {}", ev.time);

  if (ev.eventMask & PointerEventMask::Enter)
    logger::log(logL::Debug, cs.logCtx, "Pointer entered at ({}, {})",
                wl_fixed_to_double(ev.surfaceX), wl_fixed_to_double(ev.surfaceY));

  if (ev.eventMask & PointerEventMask::Leave)
    logger::log(logL::Debug, cs.logCtx, "Pointer left surface");

  if (ev.eventMask & PointerEventMask::Motion)
    logger::log(logL::Debug, cs.logCtx, "Pointer moved to ({}, {})",
                wl_fixed_to_double(ev.surfaceX), wl_fixed_to_double(ev.surfaceY));

  if (ev.eventMask & PointerEventMask::Button)
  {
    const char* stateStr = ev.state == WL_POINTER_BUTTON_STATE_RELEASED ? "released" : "pressed";
    logger::log(logL::Debug, cs.logCtx, "Button {} {}", ev.button, stateStr);
  }

  const uint32_t axisEvents = PointerEventMask::Axis | PointerEventMask::AxisSource |
                              PointerEventMask::AxisStop | PointerEventMask::AxisDiscrete;

  static constexpr CStrArray<2> axisNames   = {"vertical", "horizontal"};
  static constexpr CStrArray<4> sourceNames = {"wheel", "finger", "continuous", "wheel tilt"};

  if (ev.eventMask & axisEvents)
  {
    for (iters i = 0; i < 2; ++i)
    {
      if (!ev.axes[i].valid)
        continue;

      logger::log(logL::Debug, cs.logCtx, "{} axis event", axisNames[i]);

      if (ev.eventMask & PointerEventMask::Axis)
        logger::log(logL::Debug, cs.logCtx, "  value: {}", wl_fixed_to_double(ev.axes[i].value));

      if (ev.eventMask & PointerEventMask::AxisDiscrete)
        logger::log(logL::Debug, cs.logCtx, "  discrete: {}", ev.axes[i].discrete);

      if (ev.eventMask & PointerEventMask::AxisSource)
        logger::log(logL::Debug, cs.logCtx, "  source: {}", sourceNames[ev.axisSource]);

      if (ev.eventMask & PointerEventMask::AxisStop)
        logger::log(logL::Info, cs.logCtx, "  axis stopped");
    }
  }

  ev = PointerEvent{}; // safely reset
}

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
