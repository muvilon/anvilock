#ifndef ANVLK_WIDGETS_PASSWORD_FIELD_HPP
#define ANVLK_WIDGETS_PASSWORD_FIELD_HPP

#include <anvilock/include/utils/Assert.hpp>
#include <anvilock/include/widgets/WidgetInterface.hpp>
#include <anvilock/include/widgets/helpers/WidgetHelper.hpp>

namespace anvlk::widgets
{

auto renderPasswordField(ClientState& state) -> WidgetRegistryStatus;

}

#endif
