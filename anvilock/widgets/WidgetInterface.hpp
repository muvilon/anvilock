#ifndef ANVLK_WIDGETS_WIDGET_INTERFACE_HPP
#define ANVLK_WIDGETS_WIDGET_INTERFACE_HPP

#include <anvilock/ClientState.hpp>
#include <anvilock/LogMacros.hpp>
#include <functional>
#include <string>
#include <unordered_map>

#define ANVLK_WIDGET_REGISTRY extern

anvlk::types::WidgetName _BackgroundWidgetName    = "background";
anvlk::types::WidgetName _PasswordFieldWidgetName = "passwordfield";

namespace anvlk::widgets
{

using WidgetRegistryStatus = std::pair<GLuint, bool>;

using WidgetFunction = std::function<WidgetRegistryStatus(ClientState&)>;

class WidgetFunctionRegistry
{
public:
  static auto instance() -> WidgetFunctionRegistry&;

  void registerLoader(const std::string& name, WidgetFunction func);
  auto getLoader(const std::string& name) const -> WidgetFunction;

private:
  std::unordered_map<std::string, WidgetFunction> loaders;
};

} // namespace anvlk::widgets

#endif
