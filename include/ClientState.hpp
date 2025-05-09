#pragma once

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <anvilock/include/Log.hpp>
#include <anvilock/include/Types.hpp>
#include <anvilock/include/config/ConfigStruct.hpp>
#include <anvilock/include/freetype/FreeTypeStruct.hpp>
#include <anvilock/include/pam/PamAuthenticator.hpp>
#include <anvilock/include/shaders/ShaderHandler.hpp>
#include <anvilock/protocols/ext-session-lock-client-protocol.h>
#include <anvilock/protocols/xdg-shell-client-protocol.h>
#include <array>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <xkbcommon/xkbcommon.h>

using namespace anvlk::types;

// Custom defs
namespace anvlk::types::ext
{
using SessionLockManagerV1_ = struct ext_session_lock_manager_v1;
using SessionLockObjV1_     = struct ext_session_lock_v1;
using SessionLockSurfaceV1_ = struct ext_session_lock_surface_v1;
} // namespace anvlk::types::ext

namespace anvlk::types::wayland
{
// Wayland Defs
using WLDisplay_    = struct wl_display;
using WLRegistry_   = struct wl_registry;
using WLShm_        = struct wl_shm;
using WLShmPool_    = struct wl_shm_pool;
using WLCompositor_ = struct wl_compositor;
using WLSeat_       = struct wl_seat;
using WLOutput_     = struct wl_output;
using WLSurface_    = struct wl_surface;
using WLKeyboard_   = struct wl_keyboard;
using WLPointer_    = struct wl_pointer;
using WLArray_      = struct wl_array;
using WLFixed_      = wl_fixed_t;
using WLBuffer_     = wl_buffer;
} // namespace anvlk::types::wayland

namespace anvlk::types::wayland::egl
{
using WLEglWindow_ = struct wl_egl_window;
}

namespace anvlk::types::wayland::xdg
{
using XDGWmBase_   = struct xdg_wm_base;
using XDGSurface_  = struct xdg_surface;
using XDGTopLevel_ = struct xdg_toplevel;
} // namespace anvlk::types::wayland::xdg

namespace anvlk::types::xkb
{
using XKBState_  = xkb_state;
using XKBCtx_    = xkb_context;
using XKBKeyMap_ = xkb_keymap;
using XKBKeySym_ = xkb_keysym_t;
} // namespace anvlk::types::xkb

// Stores output-related information
struct OutputState
{
  u32                               id{};
  Dimensions                        width{};
  Dimensions                        height{};
  i32                               refresh_rate{};
  anvlk::types::wayland::WLOutput_* wlOutput = nullptr;
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
  bool                        surfaceCreated = false;
  bool                        surfaceDirty   = false;
  ext::SessionLockManagerV1_* lockManager;
  ext::SessionLockObjV1_*     lockObj;
  ext::SessionLockSurfaceV1_* lockSurface;
};

struct AuthState
{
  AuthFlag authSuccess = false;
  AuthFlag authFailed  = false;
  float    failEffectIntensity{};
  float    successEffectIntensity{};
};

struct KeyboardState
{
  bool      ctrlHeld          = false;
  bool      backspaceHeld     = false;
  TimePoint lastBackspaceTime = SteadyClock::now();

public:
  void resetState();
};

struct PamState
{
  bool                         firstEnterPress = true;
  AuthString                   username;
  AuthString                   password;
  int                          passwordIndex = 0;
  bool                         locked        = false;
  AuthState                    authState;
  static constexpr std::size_t MAX_PASSWORD_LENGTH = 128;

public:
  auto canSeekIndex() -> bool;
  auto canSeekToOffset(const i64& idx) -> bool;
  void clearPassword();
  void seekToIndex(const i64& idx);
};

struct ClientState
{
  /* Wayland Globals */
  wayland::WLDisplay_*    wlDisplay    = nullptr;
  wayland::WLRegistry_*   wlRegistry   = nullptr;
  wayland::WLShm_*        wlShm        = nullptr;
  wayland::WLShmPool_*    wlShmPool    = nullptr;
  wayland::WLCompositor_* wlCompositor = nullptr;
  wayland::WLSeat_*       wlSeat       = nullptr;
  wayland::WLOutput_*     wlOutput     = nullptr;
  wayland::WLKeyboard_*   wlKeyboard   = nullptr;
  wayland::WLPointer_*    wlPointer    = nullptr;
  wayland::WLSurface_*    wlSurface    = nullptr;

  wayland::egl::WLEglWindow_* eglWindow = nullptr;

  /* XDG Objects */
  wayland::xdg::XDGWmBase_*   xdgWmBase   = nullptr;
  wayland::xdg::XDGSurface_*  xdgSurface  = nullptr;
  wayland::xdg::XDGTopLevel_* xdgToplevel = nullptr;

  // Window state
  Dimensions width  = 0;
  Dimensions height = 0;
  bool       closed = false;

  // Filesystem directories
  Directory homeDir;
  Directory shaderRuntimeDir;

  // Input state
  PointerEvent pointerEvent;

  KeyboardState keyboardState;

  // XKB keyboard state (raw pointers for now)
  xkb::XKBState_*  xkbState   = nullptr;
  xkb::XKBCtx_*    xkbContext = nullptr;
  xkb::XKBKeyMap_* xkbKeymap  = nullptr;

  // FreeTypeState
  FreeTypeState freeTypeState;

  // Animation and rendering state
  AnimationState animationState;
  float          offset    = 0.0f;
  u32            lastFrame = 0;

  // PAM and authentication
  PamState pamState;

  std::unique_ptr<anvlk::pam::PamAuthenticator> pamAuth;

  // Session lock protocol
  ExtSessionLock sessionLock;

  // Display output
  OutputState outputState;

  // Parsed user config
  AnvlkConfig userConfig;

  // EGL/GLES
  EGLDisplay eglDisplay  = EGL_NO_DISPLAY;
  EGLContext eglContext  = EGL_NO_CONTEXT;
  EGLSurface eglSurface  = EGL_NO_SURFACE;
  EGLConfig  eglConfig   = nullptr;
  GLuint     timeTexture = 0;

  // Shader Manager
  std::unique_ptr<anvlk::gfx::ShaderManager> shaderManagerPtr;

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
                     anvlk::logger::LogLevel logLevel);
  void initShaderManager();
  void initPamAuth();
  void destroyEGL();
  void destroyFreeType();
  void disconnectWLDisplay();
};
