#ifndef ANVLK_WIDGETS_BACKGROUND_TEXTURE_HPP
#define ANVLK_WIDGETS_BACKGROUND_TEXTURE_HPP

#include <GLES3/gl3.h>
#include <anvilock/widgets/WidgetInterface.hpp>
#include <anvilock/widgets/helpers/WidgetHelper.hpp>

namespace anvlk::widgets
{

auto loadBGTexture(ClientState& cs) -> WidgetRegistryStatus;

}

#endif
