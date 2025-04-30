#pragma once

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/Log.hpp>
#include <wayland-client.h>

namespace anvlk::wl
{

using logL = logger::LogLevel;

inline void handleOutputGeometry(void* data, wl_output*, i32 x, i32 y, int32_t physicalWidth,
                                 i32 physicalHeight, [[maybe_unused]] i32 subpixel,
                                 const char* make, const char* model,
                                 [[maybe_unused]] i32 transform)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::log(logL::Info, cs.logCtx, "Output: {} {} @ ({}, {}) [{} x {}] mm", make, model, x, y,
              physicalWidth, physicalHeight);
}

inline void handleOutputMode(void* data, wl_output*, [[maybe_unused]] u32 flags, i32 width,
                             i32 height, i32 refreshRate)
{
  auto& cs                    = *static_cast<ClientState*>(data);
  cs.outputState.width        = width;
  cs.outputState.height       = height;
  cs.outputState.refresh_rate = refreshRate;

  logger::log(logL::Info, cs.logCtx, "Output mode: {}x{} @ {} Hz", width, height, refreshRate);
}

inline void handleOutputScale(void* data, wl_output*, i32 factor)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::log(logL::Info, cs.logCtx, "Output scale factor: {}", factor);
}

inline void handleOutputDone(void* data, wl_output*)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::log(logL::Info, cs.logCtx, "Output configuration done.");
}

inline void handleOutputName(void* data, wl_output*, const char* name)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::log(logL::Info, cs.logCtx, "Output name: {}", name);
}

inline void handleOutputDescription(void* data, wl_output*, const char* description)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::log(logL::Info, cs.logCtx, "Output description: {}", description);
}

// Output listener
inline constexpr wl_output_listener kOutputListener{
  .geometry    = handleOutputGeometry,
  .mode        = handleOutputMode,
  .done        = handleOutputDone,
  .scale       = handleOutputScale,
  .name        = handleOutputName,
  .description = handleOutputDescription,
};

// Register the wl_output global and set listener
inline void registerOutput(ClientState& cs, wl_registry* registry, u32 id, u32 version)
{
  if (cs.outputState.wlOutput)
  {
    logger::log(logL::Error, cs.logCtx, "Output already bound. Skipping re-registration.");
    return;
  }

  cs.outputState.wlOutput =
    static_cast<wl_output*>(wl_registry_bind(registry, id, &wl_output_interface, version));
  wl_output_add_listener(cs.outputState.wlOutput, &kOutputListener,
                         &cs); // Use get() to get raw pointer

  logger::log(logL::Info, cs.logCtx, "Registered wl_output with ID: {}", id);
}

} // namespace anvlk::wl
