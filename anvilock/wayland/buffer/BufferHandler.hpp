#pragma once

#include <anvilock/ClientState.hpp>
#include <anvilock/Types.hpp>
#include <wayland-client.h>

namespace anvlk::wl
{

class WLBufferHandler
{
public:
  static void onRelease([[maybe_unused]] anvlk::types::VPtr data,
                        anvlk::types::wayland::WLBuffer_*   buffer)
  {
    // Callback when the compositor is done with the buffer.
    // Typically safe to destroy the buffer here.
    if (buffer)
    {
      wl_buffer_destroy(buffer);
    }
  }

  static constexpr wl_buffer_listener kWLBufferListener{
    .release = WLBufferHandler::onRelease,
  };
};

} // namespace anvlk::wl
