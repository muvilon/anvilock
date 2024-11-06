#ifndef CLIENT_STATE_H
#define CLIENT_STATE_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdbool.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <xkbcommon/xkbcommon.h>

// Structure for storing output-related information
struct output_state
{
  uint32_t          id;
  int32_t           width;
  int32_t           height;
  int32_t           refresh_rate;
  struct wl_output* wl_output;
};

// Structure to represent pointer events and their associated state
struct pointer_axes
{
  bool       valid;    // Indicates if the axis value is valid
  wl_fixed_t value;    // Value of the axis (e.g., scroll amount)
  int32_t    discrete; // Discrete value for axis, if applicable
};

struct pointer_event
{
  uint32_t            event_mask;  // Mask for event type (button press/release, motion, etc.)
  wl_fixed_t          surface_x;   // X coordinate on the surface
  wl_fixed_t          surface_y;   // Y coordinate on the surface
  uint32_t            button;      // Button associated with the event
  uint32_t            state;       // Button state (pressed/released)
  uint32_t            time;        // Time of the event
  uint32_t            serial;      // Serial number for the event
  struct pointer_axes axes[2];     // Axes information (0 = vertical, 1 = horizontal)
  uint32_t            axis_source; // Source of the axis event (e.g., finger, wheel)
};

// Structure to handle session locking states
struct session_lock
{
  bool                                surface_created;          // Is the lock surface created?
  bool                                surface_dirty;            // Is the surface marked as dirty?
  struct ext_session_lock_manager_v1* ext_session_lock_manager; // Manager for session lock
  struct ext_session_lock_v1*         ext_session_lock;         // Session lock object
  struct ext_session_lock_surface_v1* ext_session_lock_surface; // Surface for lock
};

// Structure to store PAM-related state and authentication information
struct pam_state
{
  bool  first_enter_press; // Tracks first Enter key press for authentication
  char* username;          // Stores the username for authentication
  char  password[256];     // Password buffer
  int   password_index;    // Current index in the password buffer
  bool  authenticated;     // Tracks if user is authenticated
  bool  locked;            // Locks the session if authentication fails
};

// Main structure for client state and interaction with Wayland
struct client_state
{
  /* Wayland Globals */
  struct wl_display*    wl_display;    // Wayland display connection
  struct wl_registry*   wl_registry;   // Global registry for Wayland objects
  struct wl_shm*        wl_shm;        // Shared memory interface
  struct wl_compositor* wl_compositor; // Wayland compositor
  struct xdg_wm_base*   xdg_wm_base;   // XDG shell for window management
  struct wl_seat*       wl_seat;       // Wayland seat (input device manager)
  struct wl_output*     wl_output;     // Output display

  /* Wayland Objects */
  struct wl_surface*    wl_surface; // Wayland surface
  struct wl_egl_window* egl_window;
  struct xdg_surface*   xdg_surface;  // XDG surface for window management
  struct xdg_toplevel*  xdg_toplevel; // Top-level window interface
  struct wl_keyboard*   wl_keyboard;  // Keyboard interface
  struct wl_pointer*    wl_pointer;   // Pointer interface

  /* Window State */
  int  width, height; // Dimensions of the window surface
  bool closed;        // Window close flag

  /* Input and Pointer State */
  struct pointer_event pointer_event; // Holds the current pointer event

  /* XKB Keyboard State */
  struct xkb_state*   xkb_state;   // Keyboard state for XKB
  struct xkb_context* xkb_context; // XKB context
  struct xkb_keymap*  xkb_keymap;  // Keymap for XKB keyboard

  /* Offset and Frame Data */
  float    offset;     // Offset for rendering or transformations
  uint32_t last_frame; // Last rendered frame number

  /* PAM and Authentication State */
  struct pam_state pam; // PAM state and authentication handling

  /* Session Lock State */
  struct session_lock session_lock; // Session lock management

  /* Output State */
  struct output_state output_state; // Current output information

  /* EGL and GLES State */
  EGLDisplay egl_display; // EGL display connection
  EGLContext egl_context; // EGL rendering context
  EGLSurface egl_surface; // EGL surface for rendering
  EGLConfig  egl_config;  // EGL configuration
};

#endif
