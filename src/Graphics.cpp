#include "Graphics.h"
#include <cmath>
#include <algorithm>

namespace Graphics {
    void drawRoundedRect(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color, bool isHovered) {
        SDL_Color finalColor = isHovered ? BUTTON_HOVER : color;
        SDL_SetRenderDrawColor(renderer, finalColor.r, finalColor.g, finalColor.b, finalColor.a);
        SDL_RenderFillRect(renderer, &rect);
    }
}

SDL_Color Graphics::getTileColor(int value) {
    if (value == 0) return COLORS[0];
    int index = 1;
    while (value > 2 && index < 12) {
        value /= 2;
        index++;
    }
    return COLORS[std::min<int>(index, 11)];
}

bool Graphics::isMouseOver(const SDL_Rect& rect, int mouseX, int mouseY) {
    return mouseX >= rect.x && mouseX <= rect.x + rect.w &&
           mouseY >= rect.y && mouseY <= rect.y + rect.h;
} 