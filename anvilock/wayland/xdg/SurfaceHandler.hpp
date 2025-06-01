#ifndef ANVLK_WAYLAND_XDG_SURFACE_HANDLER_HPP
#define ANVLK_WAYLAND_XDG_SURFACE_HANDLER_HPP

#include <anvilock/ClientState.hpp>
#include <anvilock/Log.hpp>
#include <anvilock/Types.hpp>
#include <wayland-client.h>
#include <wayland-egl.h>

namespace anvlk::wl
{

// Expose the xdg_surface listener
extern const xdg_surface_listener kXdgSurfaceListener;

} // namespace anvlk::wl

#endif
