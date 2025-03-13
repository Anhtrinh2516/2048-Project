#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <vector>
#include "Constants.h"

class Game2048 {
public:
    Game2048();
    ~Game2048();
    
    bool init();
    void run();
    void cleanup();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* menuFont;
    TTF_Font* scoreFont;
    
    // Thêm các texture cho tên và điểm số
    SDL_Texture* player1NameTexture;
    SDL_Texture* player2NameTexture;
    SDL_Texture* score1Texture;
    SDL_Texture* score2Texture;
    SDL_Rect player1NameRect;
    SDL_Rect player2NameRect;
    SDL_Rect score1Rect;
    SDL_Rect score2Rect;
    int lastScore1;
    int lastScore2;
    
    // Game state
    std::vector<std::vector<int>> board;
    std::vector<std::vector<int>> board2;
    std::vector<std::vector<int>> previousBoard;
    std::vector<std::vector<int>> previousBoard2;
    int score;
    int score2;
    int previousScore;
    int previousScore2;
    int bestScore;
    bool gameOver;
    bool gameOver2;
    bool inMenu;
    bool firstGame;
    bool isMultiplayer;
    
    // Game functions
    void initializeBoard();
    void initializeMultiplayerBoards();
    void initializeNewGame();
    void addNewTile();
    void addNewTilePlayer2();
    bool canMove();
    bool canMovePlayer2();
    bool moveTiles(int dx, int dy);
    bool moveTilesPlayer2(int dx, int dy);
    void mergeTiles(int dx, int dy);
    void mergeTilesPlayer2(int dx, int dy);
    void updateBestScore();
    
    // Drawing functions
    void render(int mouseX, int mouseY);
    void drawMenu();
    void drawMultiplayerBoards();
    void drawBoard(std::vector<std::vector<int>>& board, int boardX, int boardY);
    void drawTile(int value, SDL_Rect rect);
    void drawScore(const char* label, int value, int x, int y);
    void drawButton(const std::string& text, SDL_Rect rect);
    void drawBoardOnly(std::vector<std::vector<int>>& board, int boardX, int boardY);
    
    // Helper functions
    SDL_Color getTileColor(int value);
    bool isMouseOverButton(int mouseX, int mouseY, SDL_Rect buttonRect);
    void handleInput();
    void saveGame();
    void loadGame();
    void drawRoundedRect(SDL_Rect rect, SDL_Color color, int radius);
}; 