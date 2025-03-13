#include "Graphics.h"
#include <cmath>
#include <algorithm>

void Graphics::drawRoundedRect(SDL_Renderer* renderer, const SDL_Rect& rect, const SDL_Color& color, bool isHovered) {
    SDL_Color finalColor = isHovered ? BUTTON_HOVER : color;
    SDL_SetRenderDrawColor(renderer, finalColor.r, finalColor.g, finalColor.b, finalColor.a);
    
    int radius = CORNER_RADIUS;
    
    // Vẽ phần chính giữa
    SDL_Rect centerRect = {
        rect.x + radius,
        rect.y,
        rect.w - 2 * radius,
        rect.h
    };
    SDL_RenderFillRect(renderer, &centerRect);
    
    // Vẽ hai bên
    SDL_Rect leftRect = {rect.x, rect.y + radius, radius, rect.h - 2 * radius};
    SDL_Rect rightRect = {rect.x + rect.w - radius, rect.y + radius, radius, rect.h - 2 * radius};
    SDL_RenderFillRect(renderer, &leftRect);
    SDL_RenderFillRect(renderer, &rightRect);
    
    // Vẽ các góc bo tròn
    for (int i = 0; i <= radius; i++) {
        for (int j = 0; j <= radius; j++) {
            float distance = sqrt((float)(i * i + j * j));
            if (distance <= radius) {
                SDL_RenderDrawPoint(renderer, rect.x + radius - i, rect.y + radius - j);
                SDL_RenderDrawPoint(renderer, rect.x + rect.w - radius + i - 1, rect.y + radius - j);
                SDL_RenderDrawPoint(renderer, rect.x + radius - i, rect.y + rect.h - radius + j - 1);
                SDL_RenderDrawPoint(renderer, rect.x + rect.w - radius + i - 1, rect.y + rect.h - radius + j - 1);
            }
        }
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