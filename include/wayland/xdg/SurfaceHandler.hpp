#ifndef ANVLK_WAYLAND_XDG_SURFACE_HANDLER_HPP
#define ANVLK_WAYLAND_XDG_SURFACE_HANDLER_HPP

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <wayland-client.h>
#include <wayland-egl.h>

namespace anvlk::wl
{

// Expose the xdg_surface listener
extern const xdg_surface_listener kXdgSurfaceListener;

} // namespace anvlk::wl

#endif
