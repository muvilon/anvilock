#ifndef ANVLK_WIDGETS_PASSWORD_FIELD_HPP
#define ANVLK_WIDGETS_PASSWORD_FIELD_HPP

#include <anvilock/Types.hpp>
#include <anvilock/utils/Assert.hpp>
#include <anvilock/widgets/WidgetInterface.hpp>
#include <anvilock/widgets/helpers/WidgetHelper.hpp>

namespace anvlk::widgets
{

inline constexpr types::FloatArray<4> PASSWORD_FAIL_BG_COLOR = {0.98f, 0.94f, 0.94f, 0.95f};

auto renderPasswordField(ClientState& state) -> WidgetRegistryStatus;

} // namespace anvlk::widgets

#endif
