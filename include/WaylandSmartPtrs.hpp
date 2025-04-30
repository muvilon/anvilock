#pragma once

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <anvilock/protocols/ext-session-lock-client-protocol.h>
#include <anvilock/protocols/xdg-shell-client-protocol.h>
#include <memory>
#include <wayland-client.h>
#include <wayland-egl.h>

// Generic deleter template
template <typename T> struct WlDeleter
{
  void operator()(T* ptr) const noexcept
  {
    if constexpr (std::is_same_v<T, wl_display>)
      wl_display_disconnect(ptr);
    else if constexpr (std::is_same_v<T, wl_registry>)
      wl_registry_destroy(ptr);
    else if constexpr (std::is_same_v<T, wl_compositor>)
      wl_compositor_destroy(ptr);
    else if constexpr (std::is_same_v<T, wl_shm>)
      wl_shm_destroy(ptr);
    else if constexpr (std::is_same_v<T, wl_shm_pool>)
      wl_shm_pool_destroy(ptr);
    else if constexpr (std::is_same_v<T, wl_surface>)
      wl_surface_destroy(ptr);
    else if constexpr (std::is_same_v<T, wl_seat>)
      wl_seat_destroy(ptr);
    else if constexpr (std::is_same_v<T, wl_pointer>)
      wl_pointer_destroy(ptr);
    else if constexpr (std::is_same_v<T, wl_keyboard>)
      wl_keyboard_destroy(ptr);
    else if constexpr (std::is_same_v<T, wl_output>)
      wl_output_destroy(ptr);
    else if constexpr (std::is_same_v<T, xdg_wm_base>)
      xdg_wm_base_destroy(ptr);
    else if constexpr (std::is_same_v<T, xdg_surface>)
      xdg_surface_destroy(ptr);
    else if constexpr (std::is_same_v<T, xdg_toplevel>)
      xdg_toplevel_destroy(ptr);
    else if constexpr (std::is_same_v<T, wl_egl_window>)
      wl_egl_window_destroy(ptr);
    else if constexpr (std::is_same_v<T, ext_session_lock_manager_v1>)
      ext_session_lock_manager_v1_destroy(ptr);
    else if constexpr (std::is_same_v<T, ext_session_lock_v1>)
      ext_session_lock_v1_destroy(ptr);
    else if constexpr (std::is_same_v<T, ext_session_lock_surface_v1>)
      ext_session_lock_surface_v1_destroy(ptr);
    else
      static_assert(sizeof(T) == 0, "No deleter defined for this Wayland type");
  }
};

// Reusable smart pointer for all Wayland types
template <typename T> using WlUniquePtr = std::unique_ptr<T, WlDeleter<T>>;

// Factory helpers (optional, for clarity)
template <typename T> inline auto MakeWlUnique(T* raw) -> WlUniquePtr<T>
{
  return WlUniquePtr<T>{raw};
}
