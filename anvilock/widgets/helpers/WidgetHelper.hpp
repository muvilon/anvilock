#ifndef ANVLK_WIDGETS_HELPERS_WIDGET_HELPER_HPP
#define ANVLK_WIDGETS_HELPERS_WIDGET_HELPER_HPP

#include <anvilock/ClientState.hpp>
#include <anvilock/GlobalFuncs.hpp>
#include <anvilock/freetype/FreeTypeHandler.hpp>
#include <anvilock/freetype/FreeTypeStruct.hpp>

namespace anvlk::widgets::helpers
{
auto createTextTexture(ClientState& state, const std::string& text) -> GLuint;
}

#endif
