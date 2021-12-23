#ifndef TEXTURES_H
#define TEXTURES_H

#include <SDL2/SDL.h>

extern SDL_Texture *tileset, *unitTex, *uiTex, *fontTex;

void loadTextures();
void freeTextures();

#endif
