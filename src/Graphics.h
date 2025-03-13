#pragma once

#include <SDL2/SDL.h>
#include "Constants.h"

class Graphics {
public:
    static void drawRoundedRect(SDL_Renderer* renderer, const SDL_Rect& rect, const SDL_Color& color, bool isHovered = false);
    static SDL_Color getTileColor(int value);
    static bool isMouseOver(const SDL_Rect& rect, int mouseX, int mouseY);
}; 