#ifndef CLIENT_STATE_H
#define CLIENT_STATE_H

#include <stdbool.h>
#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

// Define a structure to store output-related information
struct output_state
{
  uint32_t          id;
  int32_t           width;
  int32_t           height;
  int32_t           refresh_rate;
  struct wl_output* wl_output;
};

struct pointer_event
{
  uint32_t   event_mask;           // Masks for various event types
  wl_fixed_t surface_x, surface_y; // Pointer coordinates on the surface
  uint32_t   button, state;        // Button and its state (pressed/released)
  uint32_t   time;                 // Time of the event
  uint32_t   serial;               // Serial number of the event
  struct
  {
    bool       valid;    // Validity of axis value
    wl_fixed_t value;    // Value of the axis (e.g., scroll)
    int32_t    discrete; // Discrete value (for axis)
  } axes[2];             // Array for two axes (vertical, horizontal)
  uint32_t axis_source;  // Source of the axis event
};

/* Wayland code */
struct client_state
{
  /* Globals */
  struct wl_display*    wl_display;    // Wayland display connection
  struct wl_registry*   wl_registry;   // Global registry for Wayland objects
  struct wl_shm*        wl_shm;        // Shared memory object
  struct wl_compositor* wl_compositor; // Compositor interface
  struct xdg_wm_base*   xdg_wm_base;   // XDG window manager base interface
  struct wl_seat*       wl_seat;       // Input device seat
  struct wl_output*     wl_output;
  /* Objects */
  struct wl_surface*   wl_surface;   // Wayland surface
  struct xdg_surface*  xdg_surface;  // XDG surface
  struct xdg_toplevel* xdg_toplevel; // Top-level window
  struct wl_keyboard*  wl_keyboard;  // Keyboard object
  struct wl_pointer*   wl_pointer;   // Pointer object
  /* State */
  float                offset;        // Offset for drawing (not used here)
  uint32_t             last_frame;    // Last frame number (not used here)
  int                  width, height; // Width and height of the surface
  bool                 closed;        // Flag for window closure
  struct pointer_event pointer_event; // Structure to store current pointer event
  struct xkb_state*    xkb_state;     // Keyboard state
  struct xkb_context*  xkb_context;   // XKB context for keyboard handling
  struct xkb_keymap*   xkb_keymap;    // Keymap for keyboard
  /* PAM */
  bool  firstEnterPress;
  char* username;
  char  password[256];
  int   password_index;
  bool  authenticated;
  bool  locked;
  /* SESSION LOCK */
  bool                                surface_created;
  bool                                surface_dirty;
  struct ext_session_lock_manager_v1* ext_session_lock_manager_v1;
  struct ext_session_lock_v1*         ext_session_lock_v1;
  struct ext_session_lock_surface_v1* ext_session_lock_surface_v1;
  /* wl_output */
  struct output_state output_state;
};

#endif
