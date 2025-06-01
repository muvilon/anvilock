#include <anvilock/LogMacros.hpp>
#include <anvilock/wayland/seats/SeatHandler.hpp>

namespace anvlk::wl
{

void onSeatCapabilities(types::VPtr data, [[maybe_unused]] types::wayland::WLSeat_* seat,
                        uint32_t capabilities)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_SEAT);

  const bool hasPointer  = capabilities & WL_SEAT_CAPABILITY_POINTER;
  const bool hasKeyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

  // Pointer handling
  if (hasPointer && !cs.wlPointer)
  {
    cs.wlPointer = wl_seat_get_pointer(cs.wlSeat);
    if (cs.wlPointer)
    {
      wl_pointer_add_listener(cs.wlPointer, &kPointerListener, &cs);
      LOG::INFO(cs.logCtx, "Pointer capabilities enabled.");
    }
  }
  else if (!hasPointer && cs.wlPointer)
  {
    cs.wlPointer = nullptr;
    LOG::INFO(cs.logCtx, "Pointer capabilities removed.");
  }

  // Keyboard handling
  if (hasKeyboard && !cs.wlKeyboard)
  {
    cs.wlKeyboard = wl_seat_get_keyboard(cs.wlSeat);
    if (cs.wlKeyboard)
    {
      wl_keyboard_add_listener(cs.wlKeyboard, &kKeyboardListener, &cs);
      LOG::INFO(cs.logCtx, "Keyboard capabilities enabled.");
    }
  }
  else if (!hasKeyboard && cs.wlKeyboard)
  {
    wl_keyboard_release(cs.wlKeyboard);
    cs.wlKeyboard = nullptr;
    LOG::INFO(cs.logCtx, "Keyboard capabilities removed.");
  }
  logger::resetCtx(cs.logCtx);
}

void onSeatName(types::VPtr data, [[maybe_unused]] types::wayland::WLSeat_* seat, const char* name)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_SEAT);
  LOG::INFO(cs.logCtx, logger::LogStyle::BOLD, "Seat name => {}", name);
  logger::resetCtx(cs.logCtx);
}

} // namespace anvlk::wl
