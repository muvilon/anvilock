#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/LogMacros.hpp>
#include <anvilock/include/Types.hpp>
#include <wayland-client.h>

namespace anvlk::wl
{

void handleRegistryGlobal(ClientState& cs, anvlk::types::wayland::WLRegistry_* registry, u32 name,
                          const char* interface, u32 version);

void handleRegistryRemove(ClientState& cs, u32 name);

inline constexpr wl_registry_listener kRegistryListener = {
  .global = [](anvlk::types::VPtr data, anvlk::types::wayland::WLRegistry_* registry, u32 name,
               const char* interface, u32 version)
  { handleRegistryGlobal(*static_cast<ClientState*>(data), registry, name, interface, version); },
  .global_remove = [](anvlk::types::VPtr data, anvlk::types::wayland::WLRegistry_*, u32 name)
  { handleRegistryRemove(*static_cast<ClientState*>(data), name); }};

} // namespace anvlk::wl
