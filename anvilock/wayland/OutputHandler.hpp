#ifndef ANVLK_WAYLAND_OUTPUT_HANDLER_HPP
#define ANVLK_WAYLAND_OUTPUT_HANDLER_HPP

#include <anvilock/ClientState.hpp>
#include <anvilock/Types.hpp>
#include <wayland-client.h>

namespace anvlk::wl
{

// Forward declaration of the listener
extern const wl_output_listener kOutputListener;

void registerOutput(ClientState& cs, wl_registry* registry, u32 id, u32 version);

} // namespace anvlk::wl

#endif
