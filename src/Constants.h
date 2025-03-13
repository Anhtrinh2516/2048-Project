#pragma once

#include <SDL2/SDL.h>

// Game constants
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 750;
const int GRID_SIZE = 4;
const int CELL_SIZE = 115;
const int CELL_MARGIN = 15;
const int BOARD_MARGIN = (WINDOW_WIDTH - (GRID_SIZE * CELL_SIZE + (GRID_SIZE - 1) * CELL_MARGIN)) / 2;
const int CORNER_RADIUS = 8;
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 70;
const int BUTTON_MARGIN = 30;
const int SCORE_BOX_WIDTH = 140;
const int SCORE_BOX_HEIGHT = 60;
const int SCORE_BOX_MARGIN = 20;
const int MAX_HIGH_SCORES = 10;

// Animation constants
const int ANIMATION_DURATION = 100;
const int ANIMATION_STEPS = 10;
const float POPUP_SCALE = 1.0f;
const float MERGE_SCALE = 1.0f;
const float SHAKE_AMPLITUDE = 0.0f;
const float NEW_TILE_SCALE = 1.0f;

// Colors
const SDL_Color MENU_BACKGROUND = {250, 248, 239, 255};
const SDL_Color BUTTON_COLOR = {143, 122, 102, 255};
const SDL_Color BUTTON_HOVER = {133, 112, 92, 255};
const SDL_Color TITLE_COLOR = {119, 110, 101, 255};
const SDL_Color TEXT_COLOR = {249, 246, 242, 255};
const SDL_Color SCORE_BOX_COLOR = {187, 173, 160, 255};
const SDL_Color BOARD_BACKGROUND = {187, 173, 160, 255};

// Tile colors
const SDL_Color COLORS[] = {
    {205, 193, 180, 100},    // Empty cell
    {235, 220, 210, 255},    // 2
    {235, 215, 185, 255},    // 4
    {240, 167, 111, 255},    // 8
    {243, 139, 89, 255},     // 16
    {244, 114, 85, 255},     // 32
    {244, 84, 49, 255},      // 64
    {235, 197, 104, 255},    // 128
    {235, 194, 87, 255},     // 256
    {235, 190, 70, 255},     // 512
    {235, 187, 53, 255},     // 1024
    {235, 184, 36, 255}      // 2048
}; 