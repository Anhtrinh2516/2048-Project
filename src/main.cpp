#include "Game2048.h"

int main(int argc, char* argv[]) {
    Game2048 game;
    
    if (!game.init()) {
        return 1;
    }
    
    game.run();
    return 0;
} 