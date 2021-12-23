#include <SDL2/SDL.h>
#include <stdlib.h>
#include "map.h"
#include "unit.h"
#include "display.h"
#include "textures.h"
#include "building.h"

struct building *buildings = 0;
int numBuildings = 0;

int buildingMaxHP(int type) {
	switch(type) {
	default: return 50;
	}
}

const char *buildingName(int type) {
	switch(type) {
	default: return "building";
	case BTYPE_POWERPLANT: return "Power Plant";
	case BTYPE_REFINERY: return "Refinery";
	}
}

const int *buildingUnits(int type) {
	static const int refinery[] = {UNITTYPE_TRUCK, -1};
	static const int none[] = {-1};

	switch(type) {
	default: return none;
	case BTYPE_REFINERY: return refinery;
	}
}

int buildingTile(int type) {
	switch(type) {
	default: return 0;
	case BTYPE_POWERPLANT: return 5;
	case BTYPE_REFINERY: return 9;
	}
}

void addBuilding(int x, int y, int type, int team) {
	struct building b;
	b.x = x;
	b.y = y;
	b.w = 1;
	b.h = 1;
	b.spawnX = x;
	b.spawnY = y;
	b.targetX = x;
	b.targetY = y;
	b.producing = -1;
	b.progress = 0;
	b.type = type;
	b.team = team;
	b.hp = buildingMaxHP(type);
	switch(type) {
	case BTYPE_POWERPLANT:
		b.w = 2;
		b.h = 2;
		break;
	case BTYPE_REFINERY:
		b.w = 3;
		b.h = 2;
		b.spawnX++;
		b.spawnY++;
		b.targetX = b.spawnX;
		b.targetY = b.spawnY+1;
		break;
	}

	numBuildings++;
	buildings = realloc(buildings, sizeof(struct building)*numBuildings);
	buildings[numBuildings-1] = b;
}

struct building *buildingAt(int x, int y) {
	for(int i = 0; i < numBuildings; i++) {
		struct building *b = &buildings[i];
		if(x >= b->x && y >= b->y && x < b->x+b->w && y < b->y+b->h)
			return b;
	}
	return NULL;
}

void generateBuildings() {
	for(int i = 0; i < mapW*mapH; i++)
		for(int j = 0; j < BTYPE_MAX; j++) {
			int t = buildingTile(j);
			if(t == map[i] && t != 0)
				addBuilding(i%mapW, i/mapW, j, 0);
		}
}

void freeBuildings() {
	if(buildings){
		free(buildings);
		buildings = 0;
		numBuildings = 0;
	}
}

void updateBuildings(int diff) {
	for(int i = 0; i < numBuildings; i++) {
		struct building *b = &buildings[i];
		if(b->producing >= 0) {
			b->progress += unitProductionSpeed(b->producing)*diff;
			if(b->progress >= 1.0) {
				addUnit(b->spawnX, b->spawnY,
					b->producing, b->team);
				unitTarget(&units[numUnits-1],
					b->targetX, b->targetY);
				b->producing = -1;
				b->progress = 0;
			}
		}
	}
}

void drawBuildingUI(struct building *b, int x, int y) {
	SDL_Rect src = (SDL_Rect){96, 0, 8, 8};
	SDL_Rect dst = (SDL_Rect){b->x*16+x, b->y*16+y, 8, 8};
	SDL_RenderCopy(renderer, uiTex, &src, &dst);
	src.x += 8;
	dst.x += b->w*16-8;
	SDL_RenderCopy(renderer, uiTex, &src, &dst);
	src.y += 8;
	dst.y += b->h*16-8;
	SDL_RenderCopy(renderer, uiTex, &src, &dst);
	dst.x -= b->w*16-8;
	src.x -= 8;
	SDL_RenderCopy(renderer, uiTex, &src, &dst);

	src = (SDL_Rect){128, 0, 16, 16};
	dst = (SDL_Rect){b->targetX*16+x, b->targetY*16+y, 16, 16};
	SDL_RenderCopy(renderer, uiTex, &src, &dst);
}

void buildingTarget(struct building *b, int tx, int ty) {
	if(tileBlocks(tx, ty))
		return;
	b->targetX = tx;
	b->targetY = ty;
}
