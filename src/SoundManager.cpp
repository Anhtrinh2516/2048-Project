#include "SoundManager.h"
#include <iostream>

SoundManager::SoundManager() : moveSound(nullptr), mergeSound(nullptr), gameOverSound(nullptr) {}

SoundManager::~SoundManager() {
    cleanup();
}

bool SoundManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }
    
    if (!loadSound(&moveSound, "assets/sounds/move.wav") ||
        !loadSound(&mergeSound, "assets/sounds/merge.wav") ||
        !loadSound(&gameOverSound, "assets/sounds/gameover.wav")) {
        return false;
    }
    
    return true;
}

void SoundManager::cleanup() {
    if (moveSound) Mix_FreeChunk(moveSound);
    if (mergeSound) Mix_FreeChunk(mergeSound);
    if (gameOverSound) Mix_FreeChunk(gameOverSound);
    Mix_CloseAudio();
}

bool SoundManager::loadSound(Mix_Chunk** sound, const std::string& path) {
    *sound = Mix_LoadWAV(path.c_str());
    if (*sound == nullptr) {
        std::cerr << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }
    return true;
}

void SoundManager::playMove() {
    if (moveSound) Mix_PlayChannel(-1, moveSound, 0);
}

void SoundManager::playMerge() {
    if (mergeSound) Mix_PlayChannel(-1, mergeSound, 0);
}

void SoundManager::playGameOver() {
    if (gameOverSound) Mix_PlayChannel(-1, gameOverSound, 0);
} 