#pragma once

#include <SDL2/SDL.h>

// Window dimensions
const int WINDOW_WIDTH = 900;  // Giảm từ 1200 xuống 900
const int WINDOW_HEIGHT = 600;  // Giảm từ 800 xuống 600

// Grid dimensions
const int GRID_SIZE = 4;
const int CELL_SIZE = 90;  // Tăng từ 70 lên 90
const int CELL_MARGIN = 8;  // Tăng từ 6 lên 8
const int BOARD_MARGIN = 20;  // Tăng từ 15 lên 20

// Button dimensions
const int BUTTON_WIDTH = 160;  // Giảm từ 200 xuống 160
const int BUTTON_HEIGHT = 40;  // Giảm từ 50 xuống 40
const int BUTTON_MARGIN = 10;

// Score box dimensions
const int SCORE_BOX_WIDTH = 120;  // Tăng từ 100 lên 120
const int SCORE_BOX_HEIGHT = 60;  // Tăng từ 50 lên 60

// Game constants
const int CORNER_RADIUS = 8;
const int MAX_HIGH_SCORES = 10;

// Animation constants
const int ANIMATION_DURATION = 100;
const int ANIMATION_STEPS = 10;
const float POPUP_SCALE = 1.0f;
const float MERGE_SCALE = 1.0f;
const float SHAKE_AMPLITUDE = 0.0f;
const float NEW_TILE_SCALE = 1.0f;

// Colors
const SDL_Color MENU_BACKGROUND = {250, 248, 239, 255};  // Màu nền sáng
const SDL_Color BOARD_BACKGROUND = {187, 173, 160, 255}; // Màu xám cho bảng
const SDL_Color BUTTON_COLOR = {142, 122, 102, 255};    // Màu nút tối hơn
const SDL_Color BUTTON_HOVER = {156, 136, 116, 255};    // Màu nút khi hover
const SDL_Color SCORE_BOX_COLOR = {187, 173, 160, 255}; // Màu xám cho hộp điểm
const SDL_Color TEXT_COLOR = {249, 246, 242, 255};      // Màu chữ sáng
const SDL_Color TITLE_COLOR = {119, 110, 101, 255};     // Màu chữ tiêu đề tối

// Tile colors
const SDL_Color COLORS[] = {
    {205, 193, 180, 255},    // Empty cell - Xám nhạt
    {238, 228, 218, 255},    // 2 - Be nhạt
    {237, 224, 200, 255},    // 4 - Be đậm hơn
    {242, 177, 121, 255},    // 8 - Cam nhạt
    {245, 149, 99, 255},     // 16 - Cam
    {246, 124, 95, 255},     // 32 - Cam đỏ
    {246, 94, 59, 255},      // 64 - Đỏ
    {237, 207, 114, 255},    // 128 - Vàng nhạt
    {237, 204, 97, 255},     // 256 - Vàng
    {237, 200, 80, 255},     // 512 - Vàng đậm
    {237, 197, 63, 255},     // 1024 - Vàng rực
    {237, 194, 46, 255}      // 2048 - Vàng cam
}; 