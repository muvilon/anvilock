#include <anvilock/include/widgets/WidgetInterface.hpp>

namespace anvlk::widgets
{

auto WidgetFunctionRegistry::instance() -> WidgetFunctionRegistry&
{
  static WidgetFunctionRegistry inst;
  return inst;
}

void WidgetFunctionRegistry::registerLoader(const std::string& name, WidgetFunction func)
{
  loaders[name] = std::move(func);
}

auto WidgetFunctionRegistry::getLoader(const std::string& name) const -> WidgetFunction
{
  auto it = loaders.find(name);
  return (it != loaders.end()) ? it->second : nullptr;
}

} // namespace anvlk::widgets
