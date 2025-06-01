#ifndef ANVLK_WIDGETS_HELPERS_WIDGET_HELPER_HPP
#define ANVLK_WIDGETS_HELPERS_WIDGET_HELPER_HPP

#include <anvilock/include/ClientState.hpp>
#include <anvilock/include/GlobalFuncs.hpp>
#include <anvilock/include/freetype/FreeTypeHandler.hpp>
#include <anvilock/include/freetype/FreeTypeStruct.hpp>

namespace anvlk::widgets::helpers
{
auto createTextTexture(ClientState& state, const std::string& text) -> GLuint;
}

#endif
