#include "Sound.h"
#include <iostream>

Sound::Sound() : moveSound(nullptr), mergeSound(nullptr), gameOverSound(nullptr) {}

Sound::~Sound() {
    Mix_FreeChunk(moveSound);
    Mix_FreeChunk(mergeSound);
    Mix_FreeChunk(gameOverSound);
    Mix_CloseAudio();
    Mix_Quit();
}

bool Sound::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }
    
    moveSound = Mix_LoadWAV("assets/sounds/move.wav");
    mergeSound = Mix_LoadWAV("assets/sounds/merge.wav");
    gameOverSound = Mix_LoadWAV("assets/sounds/gameover.wav");
    
    if (!moveSound || !mergeSound || !gameOverSound) {
        std::cerr << "Failed to load sound effects! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }
    
    return true;
}

void Sound::playMove() {
    if (moveSound) {
        Mix_PlayChannel(-1, moveSound, 0);
    }
}

void Sound::playMerge() {
    if (mergeSound) {
        Mix_PlayChannel(-1, mergeSound, 0);
    }
}

void Sound::playGameOver() {
    if (gameOverSound) {
        Mix_PlayChannel(-1, gameOverSound, 0);
    }
} 