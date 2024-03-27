#include "audio.h"
#include <stdio.h>

Mix_Music* loadSoundtrack(const char* path){
    Mix_Music* gMusic = Mix_LoadMUS(path);
    if (gMusic == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
    }
    return gMusic;
}