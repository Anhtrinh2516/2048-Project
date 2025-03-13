#pragma once

#include <SDL2/SDL_mixer.h>
#include <string>

class Sound {
private:
    Mix_Chunk* moveSound;
    Mix_Chunk* mergeSound;
    Mix_Chunk* gameOverSound;

public:
    Sound();
    ~Sound();
    
    bool init();
    void playMove();
    void playMerge();
    void playGameOver();
}; 