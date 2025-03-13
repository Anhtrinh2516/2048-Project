#include "Game2048.h"
#include <random>
#include <fstream>
#include <algorithm>
#include <iostream>

Game2048::Game2048() : window(nullptr), renderer(nullptr), font(nullptr), menuFont(nullptr), scoreFont(nullptr),
    score(0), previousScore(0), bestScore(0), gameOver(false), inMenu(true), firstGame(true) {
    board = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    previousBoard = board;
}

Game2048::~Game2048() {
    cleanup();
}

bool Game2048::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("2048", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont("assets/fonts/arial.ttf", 56);
    menuFont = TTF_OpenFont("assets/fonts/arial.ttf", 26);
    scoreFont = TTF_OpenFont("assets/fonts/arial.ttf", 20);
    if (!font || !menuFont || !scoreFont) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    if (!soundManager.init()) {
        std::cerr << "Failed to initialize sound manager!" << std::endl;
        return false;
    }

    loadGame();
    return true;
}

void Game2048::run() {
    bool quit = false;
    int mouseX = 0, mouseY = 0;
    
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEMOTION) {
                mouseX = e.motion.x;
                mouseY = e.motion.y;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    handleInput();
                }
            } else if (e.type == SDL_KEYDOWN && !inMenu) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        if (moveTiles(-1, 0)) {
                            addNewTile();
                            soundManager.playMove();
                        }
                        break;
                    case SDLK_RIGHT:
                        if (moveTiles(1, 0)) {
                            addNewTile();
                            soundManager.playMove();
                        }
                        break;
                    case SDLK_UP:
                        if (moveTiles(0, -1)) {
                            addNewTile();
                            soundManager.playMove();
                        }
                        break;
                    case SDLK_DOWN:
                        if (moveTiles(0, 1)) {
                            addNewTile();
                            soundManager.playMove();
                        }
                        break;
                }
            }
        }
        
        render(mouseX, mouseY);
        SDL_Delay(16);
    }
    
    saveGame();
}

void Game2048::handleInput() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    if (inMenu) {
        SDL_Rect newGameButton = {
            WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2,
            WINDOW_HEIGHT / 2 - BUTTON_HEIGHT - BUTTON_MARGIN,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        
        SDL_Rect continueButton = {
            WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2,
            WINDOW_HEIGHT / 2 + BUTTON_MARGIN,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        
        if (Graphics::isMouseOver(newGameButton, mouseX, mouseY)) {
            initializeBoard();
            inMenu = false;
            firstGame = false;
        } else if (!firstGame && Graphics::isMouseOver(continueButton, mouseX, mouseY)) {
            inMenu = false;
        }
    } else {
        // Check for back button click
        SDL_Rect backButton = {
            20,  // X position
            20,  // Y position
            100, // Width
            40   // Height
        };
        
        if (Graphics::isMouseOver(backButton, mouseX, mouseY)) {
            inMenu = true;
            return;
        }
        
        if (gameOver) {
            SDL_Rect menuButton = {
                WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2,
                WINDOW_HEIGHT / 2 + 50,
                BUTTON_WIDTH,
                BUTTON_HEIGHT
            };
            
            if (Graphics::isMouseOver(menuButton, mouseX, mouseY)) {
                inMenu = true;
                gameOver = false;
            }
        }
    }
}

void Game2048::render(int mouseX, int mouseY) {
    SDL_SetRenderDrawColor(renderer, MENU_BACKGROUND.r, MENU_BACKGROUND.g, MENU_BACKGROUND.b, MENU_BACKGROUND.a);
    SDL_RenderClear(renderer);
    
    if (inMenu) {
        drawMenu(mouseX, mouseY);
    } else {
        drawBoard(mouseX, mouseY);
    }
    
    SDL_RenderPresent(renderer);
}

void Game2048::cleanup() {
    if (font) TTF_CloseFont(font);
    if (menuFont) TTF_CloseFont(menuFont);
    if (scoreFont) TTF_CloseFont(scoreFont);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void Game2048::initializeBoard() {
    board = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    previousBoard = board;
    score = 0;
    previousScore = 0;
    gameOver = false;
    addNewTile();
    addNewTile();
}

void Game2048::addNewTile() {
    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == 0) {
                emptyCells.push_back({i, j});
            }
        }
    }
    
    if (!emptyCells.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> cellDist(0, emptyCells.size() - 1);
        std::uniform_int_distribution<> valueDist(0, 9);
        
        int selectedCell = cellDist(gen);
        int row = emptyCells[selectedCell].first;
        int col = emptyCells[selectedCell].second;
        board[row][col] = valueDist(gen) < 9 ? 2 : 4;
    }
    
    if (!canMove()) {
        gameOver = true;
        soundManager.playGameOver();
    }
}

bool Game2048::canMove() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == 0) return true;
            
            if (i < GRID_SIZE - 1 && board[i][j] == board[i + 1][j]) return true;
            if (j < GRID_SIZE - 1 && board[i][j] == board[i][j + 1]) return true;
        }
    }
    return false;
}

bool Game2048::moveTiles(int dx, int dy) {
    previousBoard = board;
    previousScore = score;
    bool moved = false;
    
    if (dx != 0) {
        for (int i = 0; i < GRID_SIZE; i++) {
            std::vector<int> row;
            for (int j = 0; j < GRID_SIZE; j++) {
                int col = dx > 0 ? GRID_SIZE - 1 - j : j;
                if (board[i][col] != 0) row.push_back(board[i][col]);
            }
            
            mergeTiles(dx, dy);
            
            std::vector<int> newRow(GRID_SIZE, 0);
            for (size_t j = 0; j < row.size(); j++) {
                int col = dx > 0 ? GRID_SIZE - 1 - j : j;
                newRow[col] = row[j];
            }
            
            if (board[i] != newRow) moved = true;
            board[i] = newRow;
        }
    } else {
        for (int j = 0; j < GRID_SIZE; j++) {
            std::vector<int> col;
            for (int i = 0; i < GRID_SIZE; i++) {
                int row = dy > 0 ? GRID_SIZE - 1 - i : i;
                if (board[row][j] != 0) col.push_back(board[row][j]);
            }
            
            mergeTiles(dx, dy);
            
            std::vector<int> newCol(GRID_SIZE, 0);
            for (size_t i = 0; i < col.size(); i++) {
                int row = dy > 0 ? GRID_SIZE - 1 - i : i;
                newCol[i] = col[i];
                if (board[row][j] != newCol[i]) moved = true;
                board[row][j] = newCol[i];
            }
        }
    }
    
    return moved;
}

void Game2048::mergeTiles(int dx, int dy) {
    if (dx != 0) {
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = dx > 0 ? GRID_SIZE - 2 : 1; dx > 0 ? j >= 0 : j < GRID_SIZE - 1; j += dx > 0 ? -1 : 1) {
                if (board[i][j] != 0 && board[i][j] == board[i][j + dx]) {
                    board[i][j + dx] *= 2;
                    score += board[i][j + dx];
                    board[i][j] = 0;
                    soundManager.playMerge();
                }
            }
        }
    } else {
        for (int j = 0; j < GRID_SIZE; j++) {
            for (int i = dy > 0 ? GRID_SIZE - 2 : 1; dy > 0 ? i >= 0 : i < GRID_SIZE - 1; i += dy > 0 ? -1 : 1) {
                if (board[i][j] != 0 && board[i][j] == board[i + dy][j]) {
                    board[i + dy][j] *= 2;
                    score += board[i + dy][j];
                    board[i][j] = 0;
                    soundManager.playMerge();
                }
            }
        }
    }
    updateBestScore();
}

void Game2048::updateBestScore() {
    if (score > bestScore) {
        bestScore = score;
    }
}

void Game2048::saveGame() {
    std::ofstream file("savegame.dat", std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<char*>(&score), sizeof(score));
        file.write(reinterpret_cast<char*>(&bestScore), sizeof(bestScore));
        for (const auto& row : board) {
            file.write(reinterpret_cast<const char*>(row.data()), sizeof(int) * GRID_SIZE);
        }
        file.close();
    }
}

void Game2048::loadGame() {
    std::ifstream file("savegame.dat", std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&score), sizeof(score));
        file.read(reinterpret_cast<char*>(&bestScore), sizeof(bestScore));
        for (auto& row : board) {
            file.read(reinterpret_cast<char*>(row.data()), sizeof(int) * GRID_SIZE);
        }
        file.close();
        previousBoard = board;
        previousScore = score;
        firstGame = false;
    }
}

void Game2048::drawTile(int value, const SDL_Rect& rect, int mouseX, int mouseY) {
    SDL_Color color = Graphics::getTileColor(value);
    Graphics::drawRoundedRect(renderer, rect, color, false);
    
    if (value > 0) {
        SDL_Color textColor = value <= 4 ? TITLE_COLOR : TEXT_COLOR;
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, std::to_string(value).c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        
        int textWidth = textSurface->w;
        int textHeight = textSurface->h;
        SDL_Rect textRect = {
            rect.x + (rect.w - textWidth) / 2,
            rect.y + (rect.h - textHeight) / 2 + 8,
            textWidth,
            textHeight
        };
        
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}

void Game2048::drawScore(const char* label, int value, int x, int y) {
    SDL_Rect scoreBox = {x, y, SCORE_BOX_WIDTH, SCORE_BOX_HEIGHT};
    Graphics::drawRoundedRect(renderer, scoreBox, SCORE_BOX_COLOR, false);
    
    SDL_Color textColor = TEXT_COLOR;
    SDL_Surface* labelSurface = TTF_RenderText_Blended(scoreFont, label, textColor);
    SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
    
    SDL_Rect labelRect = {
        x + (SCORE_BOX_WIDTH - labelSurface->w) / 2,
        y + 5,
        labelSurface->w,
        labelSurface->h
    };
    
    SDL_RenderCopy(renderer, labelTexture, nullptr, &labelRect);
    
    SDL_Surface* scoreSurface = TTF_RenderText_Blended(scoreFont, std::to_string(value).c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    
    SDL_Rect scoreRect = {
        x + (SCORE_BOX_WIDTH - scoreSurface->w) / 2,
        y + SCORE_BOX_HEIGHT - scoreSurface->h - 5,
        scoreSurface->w,
        scoreSurface->h
    };
    
    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    
    SDL_FreeSurface(labelSurface);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(labelTexture);
    SDL_DestroyTexture(scoreTexture);
}

void Game2048::drawButton(const char* text, const SDL_Rect& rect, int mouseX, int mouseY) {
    bool isHovered = Graphics::isMouseOver(rect, mouseX, mouseY);
    Graphics::drawRoundedRect(renderer, rect, BUTTON_COLOR, isHovered);
    
    SDL_Color textColor = TEXT_COLOR;
    SDL_Surface* textSurface = TTF_RenderText_Blended(menuFont, text, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    
    SDL_Rect textRect = {
        rect.x + (rect.w - textSurface->w) / 2,
        rect.y + (rect.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Game2048::drawBoard(int mouseX, int mouseY) {
    // Draw back button
    SDL_Rect backButton = {
        20,  // X position
        20,  // Y position
        100, // Width
        40   // Height
    };
    drawButton("Back", backButton, mouseX, mouseY);
    
    // Draw game title
    SDL_Color titleColor = TITLE_COLOR;
    SDL_Surface* titleSurface = TTF_RenderText_Blended(font, "2048", titleColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    
    SDL_Rect titleRect = {
        WINDOW_WIDTH / 2 - titleSurface->w / 2,
        BOARD_MARGIN,
        titleSurface->w,
        titleSurface->h
    };
    SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
    
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);
    
    // Draw score boxes - Điều chỉnh vị trí Y xuống dưới nút Back
    int scoreY = backButton.y + backButton.h + 10;  // 10 pixel khoảng cách từ nút Back
    drawScore("SCORE", score, BOARD_MARGIN, scoreY);
    drawScore("BEST", bestScore, WINDOW_WIDTH - SCORE_BOX_WIDTH - BOARD_MARGIN, scoreY);
    
    // Calculate board dimensions with extra padding
    const int BOARD_PADDING = 18;  // Thêm padding cho nền bàn cờ
    SDL_Rect boardRect = {
        BOARD_MARGIN - BOARD_PADDING,
        titleRect.y + titleRect.h + BOARD_MARGIN + 20 - BOARD_PADDING,
        GRID_SIZE * CELL_SIZE + (GRID_SIZE - 1) * CELL_MARGIN + 2 * BOARD_PADDING,
        GRID_SIZE * CELL_SIZE + (GRID_SIZE - 1) * CELL_MARGIN + 2 * BOARD_PADDING
    };
    Graphics::drawRoundedRect(renderer, boardRect, BOARD_BACKGROUND, false);
    
    // Draw tiles at their original positions (không thay đổi vị trí của các ô)
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            SDL_Rect tileRect = {
                BOARD_MARGIN + j * (CELL_SIZE + CELL_MARGIN),
                titleRect.y + titleRect.h + BOARD_MARGIN + 20 + i * (CELL_SIZE + CELL_MARGIN),
                CELL_SIZE,
                CELL_SIZE
            };
            drawTile(board[i][j], tileRect, mouseX, mouseY);
        }
    }
    
    if (gameOver) {
        // Draw semi-transparent overlay
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 238, 228, 218, 200); // Light beige with alpha
        SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(renderer, &overlay);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        
        // Draw "Game Over!" text in the center
        SDL_Color gameOverColor = TITLE_COLOR;
        SDL_Surface* gameOverSurface = TTF_RenderText_Blended(font, "Game Over!", gameOverColor);
        SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
        
        SDL_Rect gameOverRect = {
            WINDOW_WIDTH / 2 - gameOverSurface->w / 2,
            WINDOW_HEIGHT / 2 - gameOverSurface->h / 2,
            gameOverSurface->w,
            gameOverSurface->h
        };
        
        SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
        SDL_FreeSurface(gameOverSurface);
        SDL_DestroyTexture(gameOverTexture);
        
        // Draw "Back to Menu" button below Game Over text
        SDL_Rect menuButton = {
            WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2,
            gameOverRect.y + gameOverRect.h + 30,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        drawButton("Back to Menu", menuButton, mouseX, mouseY);
    }
}

void Game2048::drawMenu(int mouseX, int mouseY) {
    // Draw game title
    SDL_Color titleColor = TITLE_COLOR;
    SDL_Surface* titleSurface = TTF_RenderText_Blended(font, "2048", titleColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    
    SDL_Rect titleRect = {
        WINDOW_WIDTH / 2 - titleSurface->w / 2,
        WINDOW_HEIGHT / 4,
        titleSurface->w,
        titleSurface->h
    };
    
    SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);
    
    // Draw buttons
    SDL_Rect newGameButton = {
        WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2,
        WINDOW_HEIGHT / 2 - BUTTON_HEIGHT - BUTTON_MARGIN,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    drawButton("New Game", newGameButton, mouseX, mouseY);
    
    if (!firstGame) {
        SDL_Rect continueButton = {
            WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2,
            WINDOW_HEIGHT / 2 + BUTTON_MARGIN,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        drawButton("Continue", continueButton, mouseX, mouseY);
    }
} 