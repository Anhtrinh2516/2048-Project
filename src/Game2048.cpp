#include "Game2048.h"
#include <random>
#include <fstream>
#include <algorithm>
#include <iostream>

Game2048::Game2048() : window(nullptr), renderer(nullptr), font(nullptr), menuFont(nullptr), scoreFont(nullptr),
    player1NameTexture(nullptr), player2NameTexture(nullptr), score1Texture(nullptr), score2Texture(nullptr),
    score(0), score2(0), previousScore(0), previousScore2(0), bestScore(0), lastScore1(0), lastScore2(0),
    gameOver(false), gameOver2(false), inMenu(true), firstGame(true), isMultiplayer(false) {
    board = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    board2 = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    previousBoard = board;
    previousBoard2 = board2;
}

Game2048::~Game2048() {
    cleanup();
}

bool Game2048::init() {
    std::cout << "Initializing SDL..." << std::endl;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Creating window..." << std::endl;
    window = SDL_CreateWindow("2048", 
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            WINDOW_WIDTH, WINDOW_HEIGHT, 
                            SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Creating renderer..." << std::endl;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "Initializing TTF..." << std::endl;
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    std::cout << "Loading fonts..." << std::endl;
    // Font cho số trên ô
    font = TTF_OpenFont("assets/fonts/ClearSans-Bold.ttf", 40);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Font cho menu và nút
    menuFont = TTF_OpenFont("assets/fonts/ClearSans-Bold.ttf", 20);
    if (!menuFont) {
        std::cerr << "Failed to load menu font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Font cho điểm số
    scoreFont = TTF_OpenFont("assets/fonts/ClearSans-Bold.ttf", 24);
    if (!scoreFont) {
        std::cerr << "Failed to load score font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    std::cout << "Initializing SDL_mixer..." << std::endl;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    std::cout << "Loading game state..." << std::endl;
    loadGame();

    std::cout << "Initialization complete!" << std::endl;
    return true;
}

void Game2048::run() {
    std::cout << "Starting game loop..." << std::endl;
    bool quit = false;
    int mouseX = 0, mouseY = 0;
    
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                std::cout << "Quit event received" << std::endl;
                saveGame();  // Lưu game trước khi thoát
                quit = true;
                break;  // Thoát khỏi vòng lặp sự kiện
            } else if (e.type == SDL_MOUSEMOTION) {
                mouseX = e.motion.x;
                mouseY = e.motion.y;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    handleInput();  // Xử lý click chuột
                }
            } else if (e.type == SDL_KEYDOWN && !e.key.repeat) {
                if (!inMenu) {
                    std::cout << "Key pressed: " << SDL_GetKeyName(e.key.keysym.sym) << std::endl;
                    if (!isMultiplayer) {
                        // Chế độ một người chơi
                        switch (e.key.keysym.sym) {
                            case SDLK_LEFT:
                                if (moveTiles(-1, 0)) {
                                    addNewTile();
                                    saveGame();  // Lưu sau mỗi nước đi
                                }
                                break;
                            case SDLK_RIGHT:
                                if (moveTiles(1, 0)) {
                                    addNewTile();
                                    saveGame();  // Lưu sau mỗi nước đi
                                }
                                break;
                            case SDLK_UP:
                                if (moveTiles(0, -1)) {
                                    addNewTile();
                                    saveGame();  // Lưu sau mỗi nước đi
                                }
                                break;
                            case SDLK_DOWN:
                                if (moveTiles(0, 1)) {
                                    addNewTile();
                                    saveGame();  // Lưu sau mỗi nước đi
                                }
                                break;
                            case SDLK_ESCAPE:
                                saveGame();  // Lưu game trước khi về menu
                                inMenu = true;  // Chuyển về menu
                                break;
                        }
                    } else {
                        // Chế độ hai người chơi
                        bool moved = false;
                        switch (e.key.keysym.sym) {
                            // Người chơi 1 - WASD
                            case SDLK_a:
                                if (!gameOver) {
                                    if (moveTiles(-1, 0)) {
                                        addNewTile();
                                        if (!canMove()) gameOver = true;
                                        moved = true;
                                    }
                                }
                                break;
                            case SDLK_d:
                                if (!gameOver) {
                                    if (moveTiles(1, 0)) {
                                        addNewTile();
                                        if (!canMove()) gameOver = true;
                                        moved = true;
                                    }
                                }
                                break;
                            case SDLK_w:
                                if (!gameOver) {
                                    if (moveTiles(0, -1)) {
                                        addNewTile();
                                        if (!canMove()) gameOver = true;
                                        moved = true;
                                    }
                                }
                                break;
                            case SDLK_s:
                                if (!gameOver) {
                                    if (moveTiles(0, 1)) {
                                        addNewTile();
                                        if (!canMove()) gameOver = true;
                                        moved = true;
                                    }
                                }
                                break;
                            
                            // Người chơi 2 - Phím mũi tên
                            case SDLK_LEFT:
                                if (!gameOver2) {
                                    if (moveTilesPlayer2(-1, 0)) {
                                        addNewTilePlayer2();
                                        if (!canMovePlayer2()) gameOver2 = true;
                                        moved = true;
                                    }
                                }
                                break;
                            case SDLK_RIGHT:
                                if (!gameOver2) {
                                    if (moveTilesPlayer2(1, 0)) {
                                        addNewTilePlayer2();
                                        if (!canMovePlayer2()) gameOver2 = true;
                                        moved = true;
                                    }
                                }
                                break;
                            case SDLK_UP:
                                if (!gameOver2) {
                                    if (moveTilesPlayer2(0, -1)) {
                                        addNewTilePlayer2();
                                        if (!canMovePlayer2()) gameOver2 = true;
                                        moved = true;
                                    }
                                }
                                break;
                            case SDLK_DOWN:
                                if (!gameOver2) {
                                    if (moveTilesPlayer2(0, 1)) {
                                        addNewTilePlayer2();
                                        if (!canMovePlayer2()) gameOver2 = true;
                                        moved = true;
                                    }
                                }
                                break;
                            case SDLK_ESCAPE:
                                saveGame();  // Lưu game trước khi về menu
                                inMenu = true;  // Chuyển về menu
                                break;
                        }
                        if (moved) {
                            saveGame();  // Lưu sau mỗi nước đi trong chế độ multiplayer
                        }
                    }
                }
            }
        }
        
        render(mouseX, mouseY);
        SDL_Delay(16);  // Giới hạn FPS
    }
    
    std::cout << "Game loop ended" << std::endl;
}

void Game2048::handleInput() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (inMenu) {
        // Kiểm tra click vào nút Single Player
        SDL_Rect singlePlayerButton = {
            (WINDOW_WIDTH - BUTTON_WIDTH) / 2,
            250,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        
        if (isMouseOverButton(mouseX, mouseY, singlePlayerButton)) {
            inMenu = false;
            isMultiplayer = false;
            if (firstGame) {  // Chỉ khởi tạo bàn cờ mới nếu là game đầu tiên
                initializeBoard();
                firstGame = false;
            }
            saveGame();
            return;
        }

        // Kiểm tra click vào nút Multiplayer
        SDL_Rect multiplayerButton = {
            (WINDOW_WIDTH - BUTTON_WIDTH) / 2,
            250 + BUTTON_HEIGHT + BUTTON_MARGIN,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        
        if (isMouseOverButton(mouseX, mouseY, multiplayerButton)) {
            inMenu = false;
            isMultiplayer = true;
            if (firstGame) {  // Chỉ khởi tạo bàn cờ mới nếu là game đầu tiên
                initializeMultiplayerBoards();
                firstGame = false;
            }
            saveGame();
            return;
        }
    } else {
        // Kiểm tra click vào nút Back
        SDL_Rect backButton = {
            10,  // Cách lề trái 10px
            10,  // Cách lề trên 10px
            80,  // Chiều rộng nhỏ hơn nút thông thường
            30   // Chiều cao nhỏ hơn nút thông thường
        };
        
        if (isMouseOverButton(mouseX, mouseY, backButton)) {
            std::cout << "Back button clicked" << std::endl;
            inMenu = true;  // Chỉ chuyển về menu, không thay đổi trạng thái bàn cờ
            saveGame();  // Lưu trạng thái hiện tại
            return;
        }

        // Kiểm tra click vào nút New Game
        SDL_Rect newGameButton = {
            WINDOW_WIDTH - 100,  // Cách lề phải 10px
            10,  // Cách lề trên 10px
            90,  // Chiều rộng cho nút New Game
            30   // Chiều cao nhỏ hơn nút thông thường
        };
        
        if (isMouseOverButton(mouseX, mouseY, newGameButton)) {
            std::cout << "New Game button clicked" << std::endl;
            if (isMultiplayer) {
                initializeMultiplayerBoards();
            } else {
                initializeBoard();
            }
            saveGame();  // Lưu trạng thái game mới
            return;
        }
    }
}

bool Game2048::isMouseOverButton(int mouseX, int mouseY, SDL_Rect buttonRect) {
    return mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
           mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h;
}

void Game2048::render(int mouseX, int mouseY) {
    // Xóa màn hình với màu nền
    SDL_SetRenderDrawColor(renderer, MENU_BACKGROUND.r, MENU_BACKGROUND.g, MENU_BACKGROUND.b, MENU_BACKGROUND.a);
    SDL_RenderClear(renderer);
    
    if (inMenu) {
        drawMenu();
    } else if (isMultiplayer) {
        drawMultiplayerBoards();
    } else {
        // Tính toán vị trí cho bảng một người chơi
        int boardX = (WINDOW_WIDTH - (GRID_SIZE * CELL_SIZE + (GRID_SIZE - 1) * CELL_MARGIN)) / 2;
        drawBoard(board, boardX, 100);
        
        // Hiển thị Game Over nếu trò chơi kết thúc
        if (gameOver) {
            SDL_Color titleColor = TITLE_COLOR;
            std::string gameOverText = "Game Over!";
            SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, gameOverText.c_str(), titleColor);
            SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
            
            SDL_Rect gameOverRect = {
                (WINDOW_WIDTH - gameOverSurface->w) / 2,
                WINDOW_HEIGHT - 150,
                gameOverSurface->w,
                gameOverSurface->h
            };
            
            SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
            SDL_FreeSurface(gameOverSurface);
            SDL_DestroyTexture(gameOverTexture);
            
            // Vẽ nút Back to Menu
            SDL_Rect menuButton = {
                (WINDOW_WIDTH - BUTTON_WIDTH) / 2,
                WINDOW_HEIGHT - 100,
                BUTTON_WIDTH,
                BUTTON_HEIGHT
            };
            drawButton("Back to Menu", menuButton);
        }
    }
    
    // Hiển thị kết quả
    SDL_RenderPresent(renderer);
}

void Game2048::cleanup() {
    if (score1Texture) {
        SDL_DestroyTexture(score1Texture);
        score1Texture = nullptr;
    }
    if (score2Texture) {
        SDL_DestroyTexture(score2Texture);
        score2Texture = nullptr;
    }
    if (player1NameTexture) {
        SDL_DestroyTexture(player1NameTexture);
        player1NameTexture = nullptr;
    }
    if (player2NameTexture) {
        SDL_DestroyTexture(player2NameTexture);
        player2NameTexture = nullptr;
    }
    if (scoreFont) {
        TTF_CloseFont(scoreFont);
        scoreFont = nullptr;
    }
    if (menuFont) {
        TTF_CloseFont(menuFont);
        menuFont = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    Mix_CloseAudio();
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
    try {
        std::cout << "Moving tiles for player 1..." << std::endl;
        if (dx == 0 && dy == 0) {
            std::cout << "Invalid movement direction" << std::endl;
            return false;
        }

        // Tạo bản sao của bảng và điểm số
        previousBoard = board;
        previousScore = score;
        auto tempBoard = board;
        bool moved = false;

        // Di chuyển ngang
        if (dx != 0) {
            for (int row = 0; row < GRID_SIZE; row++) {
                std::vector<int> line;
                
                // Thu thập các số khác 0
                if (dx < 0) {
                    // Di chuyển sang trái
                    for (int col = 0; col < GRID_SIZE; col++) {
                        if (tempBoard[row][col] != 0) {
                            line.push_back(tempBoard[row][col]);
                        }
                    }
                } else {
                    // Di chuyển sang phải
                    for (int col = GRID_SIZE - 1; col >= 0; col--) {
                        if (tempBoard[row][col] != 0) {
                            line.push_back(tempBoard[row][col]);
                        }
                    }
                }
                
                // Nếu không có số nào, bỏ qua hàng này
                if (line.empty()) {
                    continue;
                }
                
                // Gộp các số giống nhau
                for (size_t i = 0; i < line.size() - 1; i++) {
                    if (line[i] == line[i + 1]) {
                        line[i] *= 2;
                        score += line[i];
                        line.erase(line.begin() + i + 1);
                        updateBestScore();  // Cập nhật điểm cao nhất
                    }
                }
                
                // Điền số 0 vào cuối nếu cần
                while (line.size() < GRID_SIZE) {
                    if (dx < 0) {
                        line.push_back(0);
                    } else {
                        line.insert(line.begin(), 0);
                    }
                }
                
                // Cập nhật bảng
                for (int col = 0; col < GRID_SIZE; col++) {
                    if (tempBoard[row][col] != line[col]) {
                        moved = true;
                        tempBoard[row][col] = line[col];
                    }
                }
            }
        }
        // Di chuyển dọc
        else if (dy != 0) {
            for (int col = 0; col < GRID_SIZE; col++) {
                std::vector<int> line;
                
                // Thu thập các số khác 0
                if (dy < 0) {
                    // Di chuyển lên
                    for (int row = 0; row < GRID_SIZE; row++) {
                        if (tempBoard[row][col] != 0) {
                            line.push_back(tempBoard[row][col]);
                        }
                    }
                } else {
                    // Di chuyển xuống
                    for (int row = GRID_SIZE - 1; row >= 0; row--) {
                        if (tempBoard[row][col] != 0) {
                            line.push_back(tempBoard[row][col]);
                        }
                    }
                }
                
                // Nếu không có số nào, bỏ qua cột này
                if (line.empty()) {
                    continue;
                }
                
                // Gộp các số giống nhau
                for (size_t i = 0; i < line.size() - 1; i++) {
                    if (line[i] == line[i + 1]) {
                        line[i] *= 2;
                        score += line[i];
                        line.erase(line.begin() + i + 1);
                        updateBestScore();  // Cập nhật điểm cao nhất
                    }
                }
                
                // Điền số 0 vào cuối nếu cần
                while (line.size() < GRID_SIZE) {
                    if (dy < 0) {
                        line.push_back(0);
                    } else {
                        line.insert(line.begin(), 0);
                    }
                }
                
                // Cập nhật bảng
                for (int row = 0; row < GRID_SIZE; row++) {
                    if (tempBoard[row][col] != line[row]) {
                        moved = true;
                        tempBoard[row][col] = line[row];
                    }
                }
            }
        }

        // Chỉ cập nhật bảng chính nếu có di chuyển
        if (moved) {
            board = tempBoard;
        }

        std::cout << "Move complete, moved = " << moved << std::endl;
        return moved;
    } catch (const std::exception& e) {
        std::cerr << "Error in moveTiles: " << e.what() << std::endl;
        return false;
    }
}

void Game2048::mergeTiles(int dx, int dy) {
    if (dx != 0) {
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = dx > 0 ? GRID_SIZE - 2 : 1; dx > 0 ? j >= 0 : j < GRID_SIZE - 1; j += dx > 0 ? -1 : 1) {
                if (board[i][j] != 0 && board[i][j] == board[i][j + dx]) {
                    board[i][j + dx] *= 2;
                    score += board[i][j + dx];
                    board[i][j] = 0;
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
                }
            }
        }
    }
    updateBestScore();
}

void Game2048::updateBestScore() {
    if (!isMultiplayer) {  // Chỉ cập nhật best score trong chế độ một người chơi
        if (score > bestScore) {
            bestScore = score;
        }
    }
}

void Game2048::saveGame() {
    std::cout << "Saving game state..." << std::endl;
    std::ofstream file("savegame.dat", std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Không thể mở file savegame.dat để ghi!" << std::endl;
        return;
    }

    try {
        // Lưu trạng thái menu và game
        bool currentInMenu = inMenu;
        file.write(reinterpret_cast<const char*>(&currentInMenu), sizeof(currentInMenu));
        if (file.fail()) throw std::runtime_error("Lỗi khi ghi inMenu");
        
        file.write(reinterpret_cast<const char*>(&firstGame), sizeof(firstGame));
        if (file.fail()) throw std::runtime_error("Lỗi khi ghi firstGame");
        
        file.write(reinterpret_cast<const char*>(&isMultiplayer), sizeof(isMultiplayer));
        if (file.fail()) throw std::runtime_error("Lỗi khi ghi isMultiplayer");
        
        // Lưu điểm số
        file.write(reinterpret_cast<const char*>(&score), sizeof(score));
        if (file.fail()) throw std::runtime_error("Lỗi khi ghi score");
        
        file.write(reinterpret_cast<const char*>(&score2), sizeof(score2));
        if (file.fail()) throw std::runtime_error("Lỗi khi ghi score2");
        
        file.write(reinterpret_cast<const char*>(&bestScore), sizeof(bestScore));
        if (file.fail()) throw std::runtime_error("Lỗi khi ghi bestScore");
        
        // Lưu trạng thái game over
        file.write(reinterpret_cast<const char*>(&gameOver), sizeof(gameOver));
        if (file.fail()) throw std::runtime_error("Lỗi khi ghi gameOver");
        
        file.write(reinterpret_cast<const char*>(&gameOver2), sizeof(gameOver2));
        if (file.fail()) throw std::runtime_error("Lỗi khi ghi gameOver2");
        
        // Lưu bảng game người chơi 1
        for (const auto& row : board) {
            file.write(reinterpret_cast<const char*>(row.data()), sizeof(int) * GRID_SIZE);
            if (file.fail()) throw std::runtime_error("Lỗi khi ghi board");
        }
        
        // Lưu bảng game người chơi 2
        for (const auto& row : board2) {
            file.write(reinterpret_cast<const char*>(row.data()), sizeof(int) * GRID_SIZE);
            if (file.fail()) throw std::runtime_error("Lỗi khi ghi board2");
        }
        
        // Lưu bảng trước đó
        for (const auto& row : previousBoard) {
            file.write(reinterpret_cast<const char*>(row.data()), sizeof(int) * GRID_SIZE);
            if (file.fail()) throw std::runtime_error("Lỗi khi ghi previousBoard");
        }
        
        for (const auto& row : previousBoard2) {
            file.write(reinterpret_cast<const char*>(row.data()), sizeof(int) * GRID_SIZE);
            if (file.fail()) throw std::runtime_error("Lỗi khi ghi previousBoard2");
        }
        
        // Lưu điểm số trước đó
        file.write(reinterpret_cast<const char*>(&previousScore), sizeof(previousScore));
        if (file.fail()) throw std::runtime_error("Lỗi khi ghi previousScore");
        
        file.write(reinterpret_cast<const char*>(&previousScore2), sizeof(previousScore2));
        if (file.fail()) throw std::runtime_error("Lỗi khi ghi previousScore2");
        
        // Đảm bảo dữ liệu được ghi xuống đĩa
        file.flush();
        if (file.fail()) throw std::runtime_error("Lỗi khi flush file");
        
        file.close();
        std::cout << "Đã lưu trạng thái game thành công!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Lỗi khi lưu game: " << e.what() << std::endl;
        file.close();
        // Xóa file nếu có lỗi để tránh dữ liệu không hoàn chỉnh
        std::remove("savegame.dat");
    }
}

void Game2048::loadGame() {
    std::cout << "Loading game state..." << std::endl;
    std::ifstream file("savegame.dat", std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Không tìm thấy file save game, bắt đầu game mới..." << std::endl;
        initializeNewGame();
        return;
    }

    try {
        // Đọc trạng thái menu và game
        file.read(reinterpret_cast<char*>(&inMenu), sizeof(inMenu));
        if (file.fail()) throw std::runtime_error("Lỗi khi đọc inMenu");
        
        file.read(reinterpret_cast<char*>(&firstGame), sizeof(firstGame));
        if (file.fail()) throw std::runtime_error("Lỗi khi đọc firstGame");
        
        file.read(reinterpret_cast<char*>(&isMultiplayer), sizeof(isMultiplayer));
        if (file.fail()) throw std::runtime_error("Lỗi khi đọc isMultiplayer");
        
        // Đọc điểm số
        file.read(reinterpret_cast<char*>(&score), sizeof(score));
        if (file.fail()) throw std::runtime_error("Lỗi khi đọc score");
        
        file.read(reinterpret_cast<char*>(&score2), sizeof(score2));
        if (file.fail()) throw std::runtime_error("Lỗi khi đọc score2");
        
        file.read(reinterpret_cast<char*>(&bestScore), sizeof(bestScore));
        if (file.fail()) throw std::runtime_error("Lỗi khi đọc bestScore");
        
        // Đọc trạng thái game over
        file.read(reinterpret_cast<char*>(&gameOver), sizeof(gameOver));
        if (file.fail()) throw std::runtime_error("Lỗi khi đọc gameOver");
        
        file.read(reinterpret_cast<char*>(&gameOver2), sizeof(gameOver2));
        if (file.fail()) throw std::runtime_error("Lỗi khi đọc gameOver2");
        
        // Đọc bảng game người chơi 1
        for (auto& row : board) {
            file.read(reinterpret_cast<char*>(row.data()), sizeof(int) * GRID_SIZE);
            if (file.fail()) throw std::runtime_error("Lỗi khi đọc board");
        }
        
        // Đọc bảng game người chơi 2
        for (auto& row : board2) {
            file.read(reinterpret_cast<char*>(row.data()), sizeof(int) * GRID_SIZE);
            if (file.fail()) throw std::runtime_error("Lỗi khi đọc board2");
        }
        
        // Đọc bảng trước đó
        for (auto& row : previousBoard) {
            file.read(reinterpret_cast<char*>(row.data()), sizeof(int) * GRID_SIZE);
            if (file.fail()) throw std::runtime_error("Lỗi khi đọc previousBoard");
        }
        
        for (auto& row : previousBoard2) {
            file.read(reinterpret_cast<char*>(row.data()), sizeof(int) * GRID_SIZE);
            if (file.fail()) throw std::runtime_error("Lỗi khi đọc previousBoard2");
        }
        
        // Đọc điểm số trước đó
        file.read(reinterpret_cast<char*>(&previousScore), sizeof(previousScore));
        if (file.fail()) throw std::runtime_error("Lỗi khi đọc previousScore");
        
        file.read(reinterpret_cast<char*>(&previousScore2), sizeof(previousScore2));
        if (file.fail()) throw std::runtime_error("Lỗi khi đọc previousScore2");
        
        file.close();
        std::cout << "Đã tải trạng thái game thành công!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Lỗi khi tải game: " << e.what() << std::endl;
        file.close();
        // Nếu có lỗi khi đọc, bắt đầu game mới
        initializeNewGame();
    }
}

void Game2048::initializeNewGame() {
    inMenu = true;
    firstGame = true;
    isMultiplayer = false;
    score = 0;
    score2 = 0;
    previousScore = 0;
    previousScore2 = 0;
    gameOver = false;
    gameOver2 = false;
    board = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    board2 = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    previousBoard = board;
    previousBoard2 = board2;
}

void Game2048::drawRoundedRect(SDL_Rect rect, SDL_Color color, int radius) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    // Vẽ phần chính giữa
    SDL_Rect innerRect = {
        rect.x + radius,
        rect.y + radius,
        rect.w - 2 * radius,
        rect.h - 2 * radius
    };
    SDL_RenderFillRect(renderer, &innerRect);
    
    // Vẽ các cạnh
    SDL_Rect sideRect = {
        rect.x + radius,
        rect.y,
        rect.w - 2 * radius,
        radius
    };
    SDL_RenderFillRect(renderer, &sideRect); // Trên
    
    sideRect.y = rect.y + rect.h - radius;
    SDL_RenderFillRect(renderer, &sideRect); // Dưới
    
    sideRect = {
        rect.x,
        rect.y + radius,
        radius,
        rect.h - 2 * radius
    };
    SDL_RenderFillRect(renderer, &sideRect); // Trái
    
    sideRect.x = rect.x + rect.w - radius;
    SDL_RenderFillRect(renderer, &sideRect); // Phải
    
    // Vẽ các góc bo tròn
    float f = 0.0;
    for (int i = 0; i <= radius; i++) {
        for (int j = 0; j <= radius; j++) {
            float distance = sqrt((float)(i * i + j * j));
            if (distance <= radius + f) {
                // Góc trên bên trái
                SDL_RenderDrawPoint(renderer, rect.x + radius - i, rect.y + radius - j);
                // Góc trên bên phải
                SDL_RenderDrawPoint(renderer, rect.x + rect.w - radius + i - 1, rect.y + radius - j);
                // Góc dưới bên trái
                SDL_RenderDrawPoint(renderer, rect.x + radius - i, rect.y + rect.h - radius + j - 1);
                // Góc dưới bên phải
                SDL_RenderDrawPoint(renderer, rect.x + rect.w - radius + i - 1, rect.y + rect.h - radius + j - 1);
            }
        }
    }
}

void Game2048::drawTile(int value, SDL_Rect rect) {
    // Vẽ background của ô với góc bo tròn
    SDL_Color color = getTileColor(value);
    drawRoundedRect(rect, color, 8);  // Tăng độ bo tròn từ 6 lên 8 để làm nét hơn

    // Nếu giá trị khác 0, vẽ số
    if (value != 0) {
        std::string text = std::to_string(value);
        SDL_Color textColor = (value <= 4) ? TITLE_COLOR : TEXT_COLOR;
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    rect.x + (rect.w - textSurface->w) / 2,
                    rect.y + (rect.h - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
    }
}

void Game2048::drawScore(const char* label, int value, int x, int y) {
    SDL_Rect scoreBox = {x, y, 100, 60};  // Giữ nguyên kích thước
    SDL_Color boxColor = {205, 193, 180, 255};  // Màu nâu nhạt
    drawRoundedRect(scoreBox, boxColor, 8);  // Tăng độ bo tròn từ 5 lên 8
    
    SDL_Color textColor = {255, 255, 255, 255};  // Màu trắng cho cả nhãn và giá trị
    
    // Vẽ nhãn với font nhỏ hơn
    SDL_Surface* labelSurface = TTF_RenderText_Solid(menuFont, label, textColor);
    if (labelSurface) {
        SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
        if (labelTexture) {
            SDL_Rect labelRect = {
                x + (scoreBox.w - labelSurface->w) / 2,
                y + 5,  // Đưa nhãn lên trên hơn
                labelSurface->w,
                labelSurface->h
            };
            SDL_RenderCopy(renderer, labelTexture, NULL, &labelRect);
            SDL_DestroyTexture(labelTexture);
        }
        SDL_FreeSurface(labelSurface);
    }
    
    // Vẽ giá trị điểm
    std::string scoreStr = std::to_string(value);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(scoreFont, scoreStr.c_str(), textColor);
    if (scoreSurface) {
        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        if (scoreTexture) {
            SDL_Rect scoreRect = {
                x + (scoreBox.w - scoreSurface->w) / 2,
                y + scoreBox.h - scoreSurface->h - 5,  // Đưa số xuống dưới hơn
                scoreSurface->w,
                scoreSurface->h
            };
            SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
            SDL_DestroyTexture(scoreTexture);
        }
        SDL_FreeSurface(scoreSurface);
    }
}

void Game2048::drawButton(const std::string& text, SDL_Rect rect) {
    // Vẽ background của nút với góc bo tròn
    SDL_Color buttonColor = BUTTON_COLOR;
    drawRoundedRect(rect, buttonColor, 8);  // Tăng độ bo tròn từ 5 lên 8

    // Vẽ text với font menuFont và màu sáng hơn
    SDL_Surface* textSurface = TTF_RenderText_Solid(menuFont, text.c_str(), TEXT_COLOR);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (textTexture) {
            SDL_Rect textRect = {
                rect.x + (rect.w - textSurface->w) / 2,
                rect.y + (rect.h - textSurface->h) / 2,
                textSurface->w,
                textSurface->h
            };
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}

void Game2048::drawBoardOnly(std::vector<std::vector<int>>& board, int boardX, int boardY) {
    // Vẽ nền của bảng với góc bo tròn
    SDL_Rect boardRect = {
        boardX - BOARD_MARGIN,
        boardY - BOARD_MARGIN,
        GRID_SIZE * CELL_SIZE + (GRID_SIZE - 1) * CELL_MARGIN + 2 * BOARD_MARGIN,
        GRID_SIZE * CELL_SIZE + (GRID_SIZE - 1) * CELL_MARGIN + 2 * BOARD_MARGIN
    };
    
    // Vẽ background của bảng với góc bo tròn
    SDL_Color boardColor = BOARD_BACKGROUND;
    drawRoundedRect(boardRect, boardColor, 8);  // Tăng độ bo tròn từ 6 lên 8

    // Vẽ từng ô trong bảng
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            SDL_Rect tileRect = {
                boardX + j * (CELL_SIZE + CELL_MARGIN),
                boardY + i * (CELL_SIZE + CELL_MARGIN),
                CELL_SIZE,
                CELL_SIZE
            };
            drawTile(board[i][j], tileRect);
        }
    }
}

void Game2048::drawBoard(std::vector<std::vector<int>>& board, int boardX, int boardY) {
    // Vẽ nút Back ở góc trên bên trái
    SDL_Rect backButton = {
        10,  // Cách lề trái 10px
        10,  // Cách lề trên 10px
        80,  // Chiều rộng nhỏ hơn nút thông thường
        35   // Chiều cao
    };
    drawButton("Back", backButton);

    // Vẽ nút New Game ở góc trên bên phải
    SDL_Rect newGameButton = {
        WINDOW_WIDTH - 100,  // Cách lề phải 10px
        10,  // Cách lề trên 10px
        90,  // Chiều rộng cho nút New Game
        30   // Chiều cao nhỏ hơn nút thông thường
    };
    drawButton("New Game", newGameButton);

    // Vẽ tiêu đề game
    SDL_Color titleColor = TITLE_COLOR;
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "2048", titleColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    
    SDL_Rect titleRect = {
        (WINDOW_WIDTH - titleSurface->w) / 2,
        10,
        titleSurface->w,
        titleSurface->h
    };
    
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Tính toán kích thước của bảng game
    int boardWidth = GRID_SIZE * CELL_SIZE + (GRID_SIZE - 1) * CELL_MARGIN;
    
    // Vẽ hộp điểm số hiện tại - đặt ở bên phải của bảng
    SDL_Rect scoreBox = {
        boardX + boardWidth - 210,  // Điều chỉnh vị trí sang trái một chút
        titleRect.y + titleRect.h + 20,  // Cách dưới tiêu đề 20px
        100,  // Chiều rộng
        60    // Chiều cao mới
    };
    drawScore("Score", score, scoreBox.x, scoreBox.y);

    // Vẽ hộp điểm cao nhất - đặt bên cạnh hộp điểm số
    SDL_Rect bestBox = {
        scoreBox.x + scoreBox.w + 10,  // Cách hộp điểm số 10px
        scoreBox.y,  // Cùng hàng với hộp điểm số
        100,  // Chiều rộng
        60    // Chiều cao mới
    };
    drawScore("Best", bestScore, bestBox.x, bestBox.y);

    // Tính toán vị trí mới cho bảng game - đặt dưới các hộp điểm số
    boardY = bestBox.y + bestBox.h + 30;  // Cách hộp điểm số 30px

    // Vẽ bảng game
    drawBoardOnly(board, boardX, boardY);
}

SDL_Color Game2048::getTileColor(int value) {
    switch (value) {
        case 0: return COLORS[0];
        case 2: return COLORS[1];
        case 4: return COLORS[2];
        case 8: return COLORS[3];
        case 16: return COLORS[4];
        case 32: return COLORS[5];
        case 64: return COLORS[6];
        case 128: return COLORS[7];
        case 256: return COLORS[8];
        case 512: return COLORS[9];
        case 1024: return COLORS[10];
        case 2048: return COLORS[11];
        default: return COLORS[11];
    }
}

void Game2048::drawMenu() {
    // Vẽ tiêu đề game với font lớn
    SDL_Color titleColor = TITLE_COLOR;
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "2048", titleColor);
    if (titleSurface) {
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        if (titleTexture) {
            SDL_Rect titleRect = {
                (WINDOW_WIDTH - titleSurface->w) / 2,
                100,
                titleSurface->w,
                titleSurface->h
            };
            SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture);
        }
        SDL_FreeSurface(titleSurface);
    }

    // Vẽ nút Single Player
    SDL_Rect singlePlayerButton = {
        (WINDOW_WIDTH - BUTTON_WIDTH) / 2,
        250,  // Di chuyển lên vị trí của Continue Game cũ
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    drawButton("Single Player", singlePlayerButton);

    // Vẽ nút Multiplayer
    SDL_Rect multiplayerButton = {
        (WINDOW_WIDTH - BUTTON_WIDTH) / 2,
        250 + BUTTON_HEIGHT + BUTTON_MARGIN,  // Điều chỉnh vị trí tương ứng
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    drawButton("Multiplayer", multiplayerButton);
}

void Game2048::initializeMultiplayerBoards() {
    std::cout << "Initializing multiplayer boards..." << std::endl;
    
    // Khởi tạo bảng cho người chơi 1
    board = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    previousBoard = board;
    score = 0;
    previousScore = 0;
    gameOver = false;
    
    // Khởi tạo bảng cho người chơi 2
    board2 = std::vector<std::vector<int>>(GRID_SIZE, std::vector<int>(GRID_SIZE, 0));
    previousBoard2 = board2;
    score2 = 0;
    previousScore2 = 0;
    gameOver2 = false;
    
    isMultiplayer = true;
    
    // Thêm 2 ô mới cho mỗi người chơi
    std::cout << "Adding initial tiles for player 1..." << std::endl;
    addNewTile();
    addNewTile();
    
    std::cout << "Adding initial tiles for player 2..." << std::endl;
    addNewTilePlayer2();
    addNewTilePlayer2();
    
    std::cout << "Multiplayer initialization complete!" << std::endl;
}

void Game2048::addNewTilePlayer2() {
    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board2[i][j] == 0) {
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
        board2[row][col] = valueDist(gen) < 9 ? 2 : 4;
    }
    
    if (!canMovePlayer2()) {
        gameOver2 = true;
    }
}

bool Game2048::canMovePlayer2() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board2[i][j] == 0) return true;
            
            if (i < GRID_SIZE - 1 && board2[i][j] == board2[i + 1][j]) return true;
            if (j < GRID_SIZE - 1 && board2[i][j] == board2[i][j + 1]) return true;
        }
    }
    return false;
}

bool Game2048::moveTilesPlayer2(int dx, int dy) {
    try {
        std::cout << "Moving tiles for player 2..." << std::endl;
        if (dx == 0 && dy == 0) {
            std::cout << "Invalid movement direction" << std::endl;
            return false;
        }

        // Tạo bản sao của bảng và điểm số
        previousBoard2 = board2;
        previousScore2 = score2;
        auto tempBoard = board2;
        bool moved = false;

        // Di chuyển ngang
        if (dx != 0) {
            for (int row = 0; row < GRID_SIZE; row++) {
                std::vector<int> line;
                
                // Thu thập các số khác 0
                for (int col = 0; col < GRID_SIZE; col++) {
                    int currentCol = (dx < 0) ? col : (GRID_SIZE - 1 - col);
                    if (tempBoard[row][currentCol] != 0) {
                        line.push_back(tempBoard[row][currentCol]);
                    }
                }
                
                // Nếu không có số nào, bỏ qua hàng này
                if (line.empty()) {
                    continue;
                }
                
                // Gộp các số giống nhau
                for (size_t i = 0; i < line.size() - 1; i++) {
                    if (line[i] == line[i + 1]) {
                        line[i] *= 2;
                        score2 += line[i];
                        line.erase(line.begin() + i + 1);
                        updateBestScore();  // Cập nhật điểm cao nhất
                        i--;
                    }
                }
                
                // Điền số 0 vào cuối nếu cần
                while (line.size() < GRID_SIZE) {
                    if (dx < 0) {
                        line.push_back(0);
                    } else {
                        line.insert(line.begin(), 0);
                    }
                }
                
                // Cập nhật bảng
                for (int col = 0; col < GRID_SIZE; col++) {
                    if (tempBoard[row][col] != line[col]) {
                        moved = true;
                        tempBoard[row][col] = line[col];
                    }
                }
            }
        }
        // Di chuyển dọc
        else if (dy != 0) {
            for (int col = 0; col < GRID_SIZE; col++) {
                std::vector<int> line;
                
                // Thu thập các số khác 0
                for (int row = 0; row < GRID_SIZE; row++) {
                    int currentRow = (dy < 0) ? row : (GRID_SIZE - 1 - row);
                    if (tempBoard[currentRow][col] != 0) {
                        line.push_back(tempBoard[currentRow][col]);
                    }
                }
                
                // Nếu không có số nào, bỏ qua cột này
                if (line.empty()) {
                    continue;
                }
                
                // Gộp các số giống nhau
                for (size_t i = 0; i < line.size() - 1; i++) {
                    if (line[i] == line[i + 1]) {
                        line[i] *= 2;
                        score2 += line[i];
                        line.erase(line.begin() + i + 1);
                        updateBestScore();  // Cập nhật điểm cao nhất
                        i--;
                    }
                }
                
                // Điền số 0 vào cuối nếu cần
                while (line.size() < GRID_SIZE) {
                    if (dy < 0) {
                        line.push_back(0);
                    } else {
                        line.insert(line.begin(), 0);
                    }
                }
                
                // Cập nhật bảng
                for (int row = 0; row < GRID_SIZE; row++) {
                    if (tempBoard[row][col] != line[row]) {
                        moved = true;
                        tempBoard[row][col] = line[row];
                    }
                }
            }
        }

        // Chỉ cập nhật bảng chính nếu có di chuyển
        if (moved) {
            board2 = tempBoard;
        }

        std::cout << "Move complete for player 2, moved = " << moved << std::endl;
        return moved;
    } catch (const std::exception& e) {
        std::cerr << "Error in moveTilesPlayer2: " << e.what() << std::endl;
        return false;
    }
}

void Game2048::mergeTilesPlayer2(int dx, int dy) {
    if (dx != 0) {
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = dx > 0 ? GRID_SIZE - 2 : 1; dx > 0 ? j >= 0 : j < GRID_SIZE - 1; j += dx > 0 ? -1 : 1) {
                if (board2[i][j] != 0 && board2[i][j] == board2[i][j + dx]) {
                    board2[i][j + dx] *= 2;
                    score2 += board2[i][j + dx];
                    board2[i][j] = 0;
                }
            }
        }
    } else {
        for (int j = 0; j < GRID_SIZE; j++) {
            for (int i = dy > 0 ? GRID_SIZE - 2 : 1; dy > 0 ? i >= 0 : i < GRID_SIZE - 1; i += dy > 0 ? -1 : 1) {
                if (board2[i][j] != 0 && board2[i][j] == board2[i + dy][j]) {
                    board2[i + dy][j] *= 2;
                    score2 += board2[i + dy][j];
                    board2[i][j] = 0;
                }
            }
        }
    }
    updateBestScore();
}

void Game2048::drawMultiplayerBoards() {
    // Vẽ nút Back ở góc trên bên trái
    SDL_Rect backButton = {
        10,  // Cách lề trái 10px
        10,  // Cách lề trên 10px
        80,  // Chiều rộng nhỏ hơn nút thông thường
        35   // Chiều cao
    };
    drawButton("Back", backButton);

    // Vẽ nút New Game ở góc trên bên phải
    SDL_Rect newGameButton = {
        WINDOW_WIDTH - 120,  // Tăng khoảng cách từ lề phải
        10,  // Cách lề trên 10px
        110,  // Tăng chiều rộng từ 100 lên 110
        40   // Tăng chiều cao từ 35 lên 40
    };
    drawButton("New Game", newGameButton);

    // Vẽ tiêu đề game
    SDL_Color titleColor = TITLE_COLOR;
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "2048 Multiplayer", titleColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    
    SDL_Rect titleRect = {
        (WINDOW_WIDTH - titleSurface->w) / 2,
        10,
        titleSurface->w,
        titleSurface->h
    };
    
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Tính toán vị trí mới cho các bàn cờ
    int totalBoardHeight = GRID_SIZE * CELL_SIZE + (GRID_SIZE - 1) * CELL_MARGIN + 2 * BOARD_MARGIN;
    int boardY = WINDOW_HEIGHT - totalBoardHeight - 30;  // Cách đáy 30px
    
    // Vẽ bảng cho Player 1 (bên trái)
    int leftBoardX = 40;  // Cách viền trái 40px
    
    // Tạo texture cho tên Player 1 nếu chưa có
    if (player1NameTexture == nullptr) {
        SDL_Surface* player1Surface = TTF_RenderText_Solid(scoreFont, "P1 (WASD)", titleColor);
        player1NameTexture = SDL_CreateTextureFromSurface(renderer, player1Surface);
        player1NameRect = {
            leftBoardX,
            80,
            player1Surface->w,
            player1Surface->h
        };
        SDL_FreeSurface(player1Surface);
    }
    
    // Cập nhật texture điểm số Player 1 nếu điểm thay đổi
    if (score != lastScore1 || score1Texture == nullptr) {
        if (score1Texture) {
            SDL_DestroyTexture(score1Texture);
        }
        std::string score1Text = "Score: " + std::to_string(score);
        SDL_Surface* score1Surface = TTF_RenderText_Solid(scoreFont, score1Text.c_str(), titleColor);
        score1Texture = SDL_CreateTextureFromSurface(renderer, score1Surface);
        score1Rect = {
            leftBoardX + player1NameRect.w + 10,
            80,
            score1Surface->w,
            score1Surface->h
        };
        SDL_FreeSurface(score1Surface);
        lastScore1 = score;
    }
    
    // Vẽ hộp nền cho Player 1
    SDL_Rect infoBox1 = {
        leftBoardX - 10,
        75,
        player1NameRect.w + score1Rect.w + 30,  // 30px padding tổng cộng
        std::max(player1NameRect.h, score1Rect.h) + 10  // 10px padding dọc
    };
    SDL_SetRenderDrawColor(renderer, SCORE_BOX_COLOR.r, SCORE_BOX_COLOR.g, SCORE_BOX_COLOR.b, SCORE_BOX_COLOR.a);
    SDL_RenderFillRect(renderer, &infoBox1);
    
    // Vẽ tên và điểm số Player 1
    SDL_RenderCopy(renderer, player1NameTexture, NULL, &player1NameRect);
    SDL_RenderCopy(renderer, score1Texture, NULL, &score1Rect);
    
    // Vẽ bảng game Player 1
    drawBoardOnly(board, leftBoardX, boardY);
    
    // Vẽ bảng cho Player 2 (bên phải)
    int rightBoardX = WINDOW_WIDTH - 40 - (GRID_SIZE * CELL_SIZE + (GRID_SIZE - 1) * CELL_MARGIN);
    
    // Tạo texture cho tên Player 2 nếu chưa có
    if (player2NameTexture == nullptr) {
        SDL_Surface* player2Surface = TTF_RenderText_Solid(scoreFont, "P2 (Arrows)", titleColor);
        player2NameTexture = SDL_CreateTextureFromSurface(renderer, player2Surface);
        player2NameRect = {
            rightBoardX,
            80,
            player2Surface->w,
            player2Surface->h
        };
        SDL_FreeSurface(player2Surface);
    }
    
    // Cập nhật texture điểm số Player 2 nếu điểm thay đổi
    if (score2 != lastScore2 || score2Texture == nullptr) {
        if (score2Texture) {
            SDL_DestroyTexture(score2Texture);
        }
        std::string score2Text = "Score: " + std::to_string(score2);
        SDL_Surface* score2Surface = TTF_RenderText_Solid(scoreFont, score2Text.c_str(), titleColor);
        score2Texture = SDL_CreateTextureFromSurface(renderer, score2Surface);
        score2Rect = {
            rightBoardX + player2NameRect.w + 10,
            80,
            score2Surface->w,
            score2Surface->h
        };
        SDL_FreeSurface(score2Surface);
        lastScore2 = score2;
    }
    
    // Vẽ hộp nền cho Player 2
    SDL_Rect infoBox2 = {
        rightBoardX - 10,
        75,
        player2NameRect.w + score2Rect.w + 30,  // 30px padding tổng cộng
        std::max(player2NameRect.h, score2Rect.h) + 10  // 10px padding dọc
    };
    SDL_SetRenderDrawColor(renderer, SCORE_BOX_COLOR.r, SCORE_BOX_COLOR.g, SCORE_BOX_COLOR.b, SCORE_BOX_COLOR.a);
    SDL_RenderFillRect(renderer, &infoBox2);
    
    // Vẽ tên và điểm số Player 2
    SDL_RenderCopy(renderer, player2NameTexture, NULL, &player2NameRect);
    SDL_RenderCopy(renderer, score2Texture, NULL, &score2Rect);
    
    // Vẽ bảng game Player 2
    drawBoardOnly(board2, rightBoardX, boardY);

    // Kiểm tra và hiển thị người chiến thắng nếu cả hai người chơi đều kết thúc
    if (gameOver && gameOver2) {
        std::string winnerText;
        if (score > score2) {
            winnerText = "Player 1 Wins!";
        } else if (score2 > score) {
            winnerText = "Player 2 Wins!";
        } else {
            winnerText = "It's a Tie!";
        }
        
        SDL_Surface* winnerSurface = TTF_RenderText_Solid(font, winnerText.c_str(), titleColor);
        SDL_Texture* winnerTexture = SDL_CreateTextureFromSurface(renderer, winnerSurface);
        
        SDL_Rect winnerRect = {
            (WINDOW_WIDTH - winnerSurface->w) / 2,
            boardY - 80,
            winnerSurface->w,
            winnerSurface->h
        };
        
        SDL_RenderCopy(renderer, winnerTexture, NULL, &winnerRect);
        SDL_FreeSurface(winnerSurface);
        SDL_DestroyTexture(winnerTexture);
        
        // Vẽ nút Back to Menu
        SDL_Rect menuButton = {
            (WINDOW_WIDTH - BUTTON_WIDTH) / 2,
            boardY - 50,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        drawButton("Back to Menu", menuButton);
    }
} 