#pragma once

#include <SDL2/SDL.h>
#include "Constants.h"

namespace Graphics {
    void drawRoundedRect(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color, bool isHovered);
    SDL_Color getTileColor(int value);
    bool isMouseOver(const SDL_Rect& rect, int mouseX, int mouseY);
} 