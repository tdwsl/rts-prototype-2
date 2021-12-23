#include <SDL2/SDL.h>
#include <assert.h>
#include "display.h"
#include "textures.h"

SDL_Texture *tileset, *unitTex, *uiTex, *fontTex;

SDL_Texture *loadTexture(const char *filename) {
	SDL_Surface *surf = SDL_LoadBMP(filename);
	assert(surf);
	SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format,255,0,255));
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	assert(tex);
	return tex;
}

void loadTextures() {
	tileset=loadTexture("img/tileset.bmp");
	unitTex=loadTexture("img/units.bmp");
	uiTex=loadTexture("img/ui.bmp");
	fontTex=loadTexture("img/font.bmp");
}

void freeTextures() {
	SDL_DestroyTexture(fontTex);
	SDL_DestroyTexture(uiTex);
	SDL_DestroyTexture(unitTex);
	SDL_DestroyTexture(tileset);
}
