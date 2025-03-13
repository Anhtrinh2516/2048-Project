#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include "Constants.h"
#include "Graphics.h"
#include "Sound.h"

class Game2048 {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* menuFont;
    TTF_Font* scoreFont;
    Sound soundManager;
    
    std::vector<std::vector<int>> board;
    std::vector<std::vector<int>> previousBoard;
    int score;
    int previousScore;
    int bestScore;
    bool gameOver;
    bool inMenu;
    bool firstGame;
    
    void initializeBoard();
    void addNewTile();
    bool canMove();
    bool moveTiles(int dx, int dy);
    void mergeTiles(int dx, int dy);
    void updateBestScore();
    void saveGame();
    void loadGame();
    
    void drawTile(int value, const SDL_Rect& rect, int mouseX, int mouseY);
    void drawScore(const char* label, int value, int x, int y);
    void drawButton(const char* text, const SDL_Rect& rect, int mouseX, int mouseY);
    void drawBoard(int mouseX, int mouseY);
    void drawMenu(int mouseX, int mouseY);
    
public:
    Game2048();
    ~Game2048();
    
    bool init();
    void run();
    void handleInput();
    void render(int mouseX, int mouseY);
    void cleanup();
}; 