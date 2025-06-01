// WidgetRegisterHelper.hpp
#pragma once

#include <anvilock/Types.hpp>
#include <anvilock/widgets/WidgetInterface.hpp>

namespace anvlk::widgets
{

template <types::WidgetNameParam Name, WidgetRegistryStatus (*Func)(ClientState&)>
struct WidgetRegisterHelper
{
  WidgetRegisterHelper() { WidgetFunctionRegistry::instance().registerLoader(Name, Func); }

  static WidgetRegisterHelper<Name, Func> instance;
};

template <types::WidgetNameParam Name, WidgetRegistryStatus (*Func)(ClientState&)>
WidgetRegisterHelper<Name, Func> WidgetRegisterHelper<Name, Func>::instance;

} // namespace anvlk::widgets
