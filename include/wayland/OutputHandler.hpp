#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Types.hpp>
#include <wayland-client.h>

namespace anvlk::wl
{

// Forward declaration of the listener
extern const wl_output_listener kOutputListener;

/**
 * @brief Registers the wl_output interface and attaches the output listener.
 *
 * @param cs Reference to the client state.
 * @param registry Wayland registry pointer.
 * @param id ID of the global object.
 * @param version Interface version.
 */
void registerOutput(ClientState& cs, wl_registry* registry, u32 id, u32 version);

} // namespace anvlk::wl
