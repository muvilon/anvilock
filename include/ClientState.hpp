#pragma once

#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/WaylandSmartPtrs.hpp>

#include <array>
#include <string>
#include <xkbcommon/xkbcommon.h>

using namespace anvlk::types;

// Stores output-related information
struct OutputState
{
  u32        id{};
  i32        width{};
  i32        height{};
  i32        refresh_rate{};
  wl_output* wlOutput = nullptr;
};

struct Vertex
{
  Coords x{}, y{}, u{}, v{};
};

struct TOMLConfig
{
  TOMLKey               fontPath;
  TOMLKey               bgName;
  TOMLKey               bgPath;
  TOMLKey               dbgLogEnable;
  TOMLKey               timeFormat;
  std::array<Vertex, 4> timeBoxVerts{};
};

struct PointerAxes
{
  bool       valid = false;
  wl_fixed_t value{};
  i32        discrete{};
};

struct PointerEvent
{
  u32                        eventMask{};
  wl_fixed_t                 surfaceX{};
  wl_fixed_t                 surfaceY{};
  u32                        button{};
  u32                        state{};
  u32                        time{};
  u32                        serial{};
  std::array<PointerAxes, 2> axes{};
  u32                        axisSource{};
};

struct AnimationState
{
  u64   frameCount{};
  u64   lastKeyFrame{};
  u64   authFailFrame{};
  float currentTime{};
  float dotBouncePhase{};
  float bgAlpha{};
  struct
  {
    Coords x{};
    Coords y{};
    float  intensity{};
    u64    start_frame{};
  } shake;
};

struct ExtSessionLock
{
  bool                                     surfaceCreated = false;
  bool                                     surfaceDirty   = false;
  WlUniquePtr<ext_session_lock_manager_v1> lockManager;
  WlUniquePtr<ext_session_lock_v1>         lockObj;
  WlUniquePtr<ext_session_lock_surface_v1> lockSurface;
};

struct AuthState
{
  AuthFlag authSuccess = false;
  AuthFlag authFailed  = false;
  float    failEffectIntensity{};
  float    successEffectIntensity{};
};

struct PamState
{
  bool       firstEnterPress = true;
  AuthString username;
  AuthString password;
  int        passwordIndex = 0;
  bool       locked        = false;
  AuthState  authState;
};

struct ClientState
{
  // Wayland globals
  WlUniquePtr<wl_display>    wlDisplay;
  WlUniquePtr<wl_registry>   wlRegistry;
  WlUniquePtr<wl_shm>        wlShm;
  WlUniquePtr<wl_shm_pool>   wlShmPool;
  WlUniquePtr<wl_compositor> wlCompositor;
  WlUniquePtr<xdg_wm_base>   xdgWMBase;
  WlUniquePtr<wl_seat>       wlSeat;

  // Some globals may still be referenced directly by other APIs
  wl_output* wlOutput = nullptr;

  // Wayland objects
  WlUniquePtr<wl_surface>    wlSurface;
  WlUniquePtr<wl_egl_window> eglWindow;
  WlUniquePtr<xdg_surface>   xdgSurface;
  WlUniquePtr<xdg_toplevel>  xdgToplevel;
  WlUniquePtr<wl_keyboard>   wlKeyboard;
  WlUniquePtr<wl_pointer>    wlPointer;

  // Window state
  int  width  = 0;
  int  height = 0;
  bool closed = false;

  // Filesystem directories
  Directory homeDir;
  Directory shaderRuntimeDir;

  // Input state
  PointerEvent pointerEvent;

  // XKB keyboard state (raw pointers for now)
  xkb_state*   xkbState   = nullptr;
  xkb_context* xkbContext = nullptr;
  xkb_keymap*  xkbKeymap  = nullptr;

  // Animation and rendering state
  AnimationState animationState;
  float          offset    = 0.0f;
  u32            lastFrame = 0;

  // PAM and authentication
  PamState pam;

  // Session lock protocol
  ExtSessionLock sessionLock;

  // Display output
  OutputState outputState;

  // Parsed user config
  TOMLConfig globalConfig;

  // EGL/GLES
  EGLDisplay eglDisplay  = EGL_NO_DISPLAY;
  EGLContext eglContext  = EGL_NO_CONTEXT;
  EGLSurface eglSurface  = EGL_NO_SURFACE;
  EGLConfig  eglConfig   = nullptr;
  GLuint     timeTexture = 0;

  // Shader program state
  struct
  {
    GLuint program             = 0;
    GLint  color_location      = -1;
    GLint  offset_location     = -1;
    GLint  time_location       = -1;
    GLint  resolution_location = -1;
    GLint  radius_location     = -1;
    GLint  position_location   = -1;
  } shader_state;

  anvlk::logger::LogContext logCtx;

public:
  void setLogContext(bool writeToFile, fsPath path, bool useTimestamp,
                     anvlk::logger::LogLevel logLevel)
  {
    logCtx = {.toFile      = writeToFile,
              .logFilePath = path,
              .timestamp   = useTimestamp,
              .minLogLevel = logLevel};

    anvlk::logger::init(logCtx);
  }
};
