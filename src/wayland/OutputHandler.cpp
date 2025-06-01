#include <anvilock/LogMacros.hpp>
#include <anvilock/wayland/OutputHandler.hpp>

namespace anvlk::wl
{

static void handleOutputGeometry(anvlk::types::VPtr data, anvlk::types::wayland::WLOutput_*, i32 x,
                                 i32 y, i32 physicalWidth, i32 physicalHeight,
                                 [[maybe_unused]] i32 subpixel, const char* make, const char* model,
                                 [[maybe_unused]] i32 transform)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_OUT);
  LOG::INFO(cs.logCtx, "Output: {} {} @ ({}, {}) [{} x {}] mm", make, model, x, y, physicalWidth,
            physicalHeight);
  logger::resetCtx(cs.logCtx);
}

static void handleOutputMode(anvlk::types::VPtr   data, anvlk::types::wayland::WLOutput_*,
                             [[maybe_unused]] u32 flags, Dimensions width, Dimensions height,
                             i32 refreshRate)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_OUT);
  cs.outputState.width        = width;
  cs.outputState.height       = height;
  cs.outputState.refresh_rate = refreshRate;

  LOG::INFO(cs.logCtx, "Output mode: {}x{} @ {} Hz", width, height, refreshRate);
  logger::resetCtx(cs.logCtx);
}

static void handleOutputScale(anvlk::types::VPtr data, anvlk::types::wayland::WLOutput_*,
                              i32                factor)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_OUT);
  LOG::INFO(cs.logCtx, "Output scale factor: {}", factor);
  logger::resetCtx(cs.logCtx);
}

static void handleOutputDone(anvlk::types::VPtr data, anvlk::types::wayland::WLOutput_*)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_OUT);
  LOG::INFO(cs.logCtx, "Output configuration done.");
  logger::resetCtx(cs.logCtx);
}

static void handleOutputName(anvlk::types::VPtr data, anvlk::types::wayland::WLOutput_*,
                             const char*        name)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_OUT);
  LOG::INFO(cs.logCtx, "Output name: {}", name);
  logger::resetCtx(cs.logCtx);
}

static void handleOutputDescription(anvlk::types::VPtr data, anvlk::types::wayland::WLOutput_*,
                                    const char*        description)
{
  auto& cs = *static_cast<ClientState*>(data);
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_OUT);
  LOG::INFO(cs.logCtx, "Output description: {}", description);
  logger::resetCtx(cs.logCtx);
}

// Listener definition
const wl_output_listener kOutputListener{
  .geometry    = handleOutputGeometry,
  .mode        = handleOutputMode,
  .done        = handleOutputDone,
  .scale       = handleOutputScale,
  .name        = handleOutputName,
  .description = handleOutputDescription,
};

void registerOutput(ClientState& cs, types::wayland::WLRegistry_* registry, u32 id, u32 version)
{
  logger::switchCtx(cs.logCtx, logger::LogCategory::WL_OUT);
  if (cs.outputState.wlOutput)
  {
    LOG::ERROR(cs.logCtx, "Output already bound. Skipping re-registration.");
    return;
  }

  cs.outputState.wlOutput = static_cast<anvlk::types::wayland::WLOutput_*>(
    wl_registry_bind(registry, id, &wl_output_interface, version));
  wl_output_add_listener(cs.outputState.wlOutput, &kOutputListener, &cs);

  LOG::INFO(cs.logCtx, "Registered anvlk::types::wayland::WLOutput_ with ID: {}", id);
}

} // namespace anvlk::wl
