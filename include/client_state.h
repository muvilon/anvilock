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

typedef struct
{
  float x, y, u, v;
} Vertex;

typedef struct
{
  char*  font_path;
  char*  bg_name;
  char*  bg_path;
  char*  debug_log_enable;
  char*  time_format;
  Vertex time_box_vertices[4];
} TOMLConfig;

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

// Structure to handle animation states and timings
struct animation_state
{
  uint64_t frame_count;      // Current frame number for animations
  uint64_t last_key_frame;   // Frame number when last key was pressed
  uint64_t auth_fail_frame;  // Frame number when authentication failed
  float    current_time;     // Current animation time in seconds
  float    dot_bounce_phase; // Phase offset for dot bounce animations
  float    background_alpha; // Background transparency animation
  struct
  {
    float    x;           // Current shake offset X
    float    y;           // Current shake offset Y
    float    intensity;   // Current shake intensity
    uint64_t start_frame; // When shake animation started
  } shake;
};

struct session_lock
{
  bool                                surface_created;
  bool                                surface_dirty;
  struct ext_session_lock_manager_v1* ext_session_lock_manager;
  struct ext_session_lock_v1*         ext_session_lock;
  struct ext_session_lock_surface_v1* ext_session_lock_surface;
};

struct auth_state
{
  bool  auth_success;
  bool  auth_failed;
  float fail_effect_intensity;    // Intensity of failure effect (0.0 - 1.0)
  float success_effect_intensity; // Intensity of success effect (0.0 - 1.0)
};

// Structure to store PAM-related state and authentication information
struct pam_state
{
  bool              first_enter_press; // Tracks first Enter key press for authentication
  char*             username;          // Stores the username for authentication
  char              password[256];     // Password buffer
  int               password_index;    // Current index in the password buffer
  bool              locked;            // Locks the session if authentication fails
  struct auth_state auth_state;        // the authentication state of the event loop
};

// Main structure for client state
struct client_state
{
  /* Wayland Globals */
  struct wl_display*    wl_display;
  struct wl_registry*   wl_registry;
  struct wl_shm*        wl_shm;
  struct wl_shm_pool*   wl_shm_pool;
  struct wl_compositor* wl_compositor;
  struct xdg_wm_base*   xdg_wm_base;
  struct wl_seat*       wl_seat;
  struct wl_output*     wl_output;

  /* Wayland Objects */
  struct wl_surface*    wl_surface;
  struct wl_egl_window* egl_window;
  struct xdg_surface*   xdg_surface;
  struct xdg_toplevel*  xdg_toplevel;
  struct wl_keyboard*   wl_keyboard;
  struct wl_pointer*    wl_pointer;

  /* Window State */
  int  width, height;
  bool closed;

  /* Input and Pointer State */
  struct pointer_event pointer_event;

  /* XKB Keyboard State */
  struct xkb_state*   xkb_state;
  struct xkb_context* xkb_context;
  struct xkb_keymap*  xkb_keymap;

  /* Animation and Rendering State */
  struct animation_state animation;
  float                  offset;
  uint32_t               last_frame;

  /* PAM and Authentication State */
  struct pam_state pam;

  /* Session Lock State */
  struct session_lock session_lock;

  /* Output State */
  struct output_state output_state;

  /* User Configs */
  TOMLConfig global_config;

  /* EGL and GLES State */
  EGLDisplay egl_display;
  EGLContext egl_context;
  EGLSurface egl_surface;
  EGLConfig  egl_config;
  GLuint     time_texture;

  /* Shader Program State */
  struct
  {
    GLuint program;
    GLint  color_location;
    GLint  offset_location;
    GLint  time_location;
    GLint  resolution_location;
    GLint  radius_location;
    GLint  position_location;
  } shader_state;
};

#endif
