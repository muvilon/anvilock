#include <anvilock/include/wayland/seats/PointerHandler.hpp>

namespace anvlk::wl
{

void onPointerEnter(types::VPtr data, types::wayland::WLPointer_*, u32 serial,
                    types::wayland::WLSurface_*, types::wayland::WLFixed_ x,
                    types::wayland::WLFixed_ y)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::Enter;
  ev.serial   = serial;
  ev.surfaceX = x;
  ev.surfaceY = y;
}

void onPointerLeave(types::VPtr data, types::wayland::WLPointer_*, u32 serial,
                    types::wayland::WLSurface_*)
{
  auto& cs = *static_cast<ClientState*>(data);
  cs.pointerEvent.eventMask |= PointerEventMask::Leave;
  cs.pointerEvent.serial = serial;
}

void onPointerMotion(types::VPtr data, types::wayland::WLPointer_*, u32 time,
                     types::wayland::WLFixed_ x, types::wayland::WLFixed_ y)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::Motion;
  ev.time     = time;
  ev.surfaceX = x;
  ev.surfaceY = y;
}

void onPointerButton(types::VPtr data, types::wayland::WLPointer_*, u32 serial, u32 time,
                     u32 button, u32 state)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::Button;
  ev.time   = time;
  ev.serial = serial;
  ev.button = button;
  ev.state  = state;
}

void onPointerAxis(types::VPtr data, types::wayland::WLPointer_*, u32 time, u32 axis,
                   types::wayland::WLFixed_ value)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::Axis;
  ev.time             = time;
  ev.axes[axis].valid = true;
  ev.axes[axis].value = value;
}

void onPointerAxisSource(types::VPtr data, types::wayland::WLPointer_*, u32 source)
{
  auto& cs = *static_cast<ClientState*>(data);
  cs.pointerEvent.eventMask |= PointerEventMask::AxisSource;
  cs.pointerEvent.axisSource = source;
}

void onPointerAxisStop(types::VPtr data, types::wayland::WLPointer_*, u32 time, u32 axis)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::AxisStop;
  ev.time             = time;
  ev.axes[axis].valid = true;
}

void onPointerAxisDiscrete(types::VPtr data, types::wayland::WLPointer_*, u32 axis, i32 discrete)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  ev.eventMask |= PointerEventMask::AxisDiscrete;
  ev.axes[axis].valid    = true;
  ev.axes[axis].discrete = discrete;
}

void onPointerFrame(types::VPtr data, types::wayland::WLPointer_*)
{
  auto& cs = *static_cast<ClientState*>(data);
  auto& ev = cs.pointerEvent;

  logger::log(logL::Trace, cs.logCtx, "Pointer frame @ {}", ev.time);

  if (ev.eventMask & PointerEventMask::Enter)
    logger::log(logL::Trace, cs.logCtx, "Pointer entered at ({}, {})",
                wl_fixed_to_double(ev.surfaceX), wl_fixed_to_double(ev.surfaceY));

  if (ev.eventMask & PointerEventMask::Leave)
    logger::log(logL::Debug, cs.logCtx, "Pointer left surface");

  if (ev.eventMask & PointerEventMask::Motion)
    logger::log(logL::Trace, cs.logCtx, "Pointer moved to ({}, {})",
                wl_fixed_to_double(ev.surfaceX), wl_fixed_to_double(ev.surfaceY));

  if (ev.eventMask & PointerEventMask::Button)
  {
    const char* stateStr = ev.state == WL_POINTER_BUTTON_STATE_RELEASED ? "released" : "pressed";
    logger::log(logL::Trace, cs.logCtx, "Button {} {}", ev.button, stateStr);
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

      logger::log(logL::Trace, cs.logCtx, "{} axis event", axisNames[i]);

      if (ev.eventMask & PointerEventMask::Axis)
        logger::log(logL::Trace, cs.logCtx, "  value: {}", wl_fixed_to_double(ev.axes[i].value));

      if (ev.eventMask & PointerEventMask::AxisDiscrete)
        logger::log(logL::Trace, cs.logCtx, "  discrete: {}", ev.axes[i].discrete);

      if (ev.eventMask & PointerEventMask::AxisSource)
        logger::log(logL::Trace, cs.logCtx, "  source: {}", sourceNames[ev.axisSource]);

      if (ev.eventMask & PointerEventMask::AxisStop)
        logger::log(logL::Info, cs.logCtx, "  axis stopped");
    }
  }

  ev = PointerEvent{}; // safely reset
}

} // namespace anvlk::wl
