#ifndef ANVLK_WIDGETS_WIDGET_REGISTRY_MACROS_HPP
#define ANVLK_WIDGETS_WIDGET_REGISTRY_MACROS_HPP

#include <anvilock/widgets/WidgetRegistryHelper.hpp>

#define REGISTER_WIDGET(NAME_LITERAL, FUNC_NAME)                                     \
  static constexpr anvlk::types::WidgetNameParam _##FUNC_NAME##_name = NAME_LITERAL; \
  static anvlk::widgets::WidgetRegisterHelper<_##FUNC_NAME##_name, FUNC_NAME>        \
    _##FUNC_NAME##_widget_reg{};

#endif
