#include <SDL2/SDL.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "textures.h"
#include "display.h"
#include "unit.h"
#include "map.h"

int *map, mapW, mapH;

void loadMap(const char *filename) {
	FILE *fp = fopen(filename, "r");
	assert(fp);
	fscanf(fp, "%d%d", &mapW, &mapH);
	map = malloc(sizeof(int)*mapW*mapH);
	for(int i = 0; i < mapW*mapH; i++)
		fscanf(fp, "%d", &map[i]);
	fclose(fp);
}

int getTile(int *map, int x, int y) {
	if(x < 0 || y < 0 || x >= mapW || y >= mapH)
		return -1;
	return map[y*mapW+x];
}

void setTile(int *map, int x, int y, int t) {
	if(x >= 0 && y >= 0 && x < mapW && y < mapH)
		map[y*mapW+x] = t;
}

void tsetTile(int *map, int x, int y, int t) {
	if(x < 0 || y < 0 || x >= mapW || y >= mapH)
		return;
	if(map[y*mapW+x])
		return;
	map[y*mapW+x] = t;
}

bool blocks(int t) {
	switch(t) {
	case 0:
		return 0;
	default:
		return 1;
	}
}

bool tileBlocks(int x, int y) {
	int t = getTile(map, x, y);
	return blocks(t);
}

void drawMap(int x, int y) {
	for(int i = 0; i < mapW*mapH; i++) {
		SDL_Rect src = (SDL_Rect){
			(map[i]%4)*16, (map[i]/4)*16, 16, 16};
		SDL_Rect dst = (SDL_Rect){(i%mapW)*16+x, (i/mapW)*16+y, 16, 16};
		SDL_RenderCopy(renderer, tileset, &src, &dst);
	}
}
