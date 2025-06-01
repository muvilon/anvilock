#ifndef ANVLK_WIDGETS_PASSWORD_FIELD_HPP
#define ANVLK_WIDGETS_PASSWORD_FIELD_HPP

#include <anvilock/utils/Assert.hpp>
#include <anvilock/widgets/WidgetInterface.hpp>
#include <anvilock/widgets/helpers/WidgetHelper.hpp>

namespace anvlk::widgets
{

auto renderPasswordField(ClientState& state) -> WidgetRegistryStatus;

}

#endif
