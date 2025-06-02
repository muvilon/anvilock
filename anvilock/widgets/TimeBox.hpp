#ifndef ANVLK_WIDGETS_TIME_BOX_HPP
#define ANVLK_WIDGETS_TIME_BOX_HPP

#include <anvilock/widgets/helpers/WidgetHelper.hpp>

namespace anvlk::widgets
{
namespace timebox
{
void updateTimeTexture(ClientState& cs);
}
void renderTimeBox(ClientState& cs, float fadeAlpha = 1.0f);
} // namespace anvlk::widgets

#endif
