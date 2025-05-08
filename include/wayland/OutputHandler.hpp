#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Types.hpp>
#include <wayland-client.h>

namespace anvlk::wl
{

// Forward declaration of the listener
extern const wl_output_listener kOutputListener;

void registerOutput(ClientState& cs, wl_registry* registry, u32 id, u32 version);

} // namespace anvlk::wl
