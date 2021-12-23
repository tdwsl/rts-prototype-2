#include <SDL2/SDL.h>
#include <stdlib.h>
#include "display.h"
#include "textures.h"
#include "map.h"
#include "path.h"
#include "defs.h"
#include "unit.h"

struct unit *units = 0;
int numUnits = 0;

int unitMaxHP(int type) {
	switch(type) {
	case UNITTYPE_SOLDIER: return 10;
	case UNITTYPE_TANK: return 30;
	case UNITTYPE_TRUCK: return 15;
	}
}

float unitSpeed(int type) {
	switch(type) {
	default: return 0.001;
	case UNITTYPE_TRUCK: return 0.002;
	case UNITTYPE_TANK: return 0.0015;
	}
}

const char *unitName(int type) {
	switch(type) {
	default: return "unit";
	case UNITTYPE_SOLDIER: return "Soldier";
	case UNITTYPE_TANK: return "Tank";
	case UNITTYPE_TRUCK: return "Gold\nTruck";
	}
}

float unitProductionSpeed(int producing) {
	switch(producing) {
	default: return 0.0001;
	case UNITTYPE_TRUCK: return 0.0002;
	case UNITTYPE_SOLDIER: return 0.0003;
	case UNITTYPE_TANK: return 0.00015;
	}
}

void addUnit(int x, int y, int type, int team) {
	struct unit u;
	u.px = x;
	u.py = y;
	u.x = x;
	u.y = y;
	u.team = team;
	u.type = type;
	u.progress = 0;
	u.hp = unitMaxHP(type);
	u.d = 4;
	u.pd = 4;
	u.targetX = 0;
	u.targetY = 0;
	u.targetMode = 0;

	numUnits++;
	units = realloc(units, sizeof(struct unit)*numUnits);
	units[numUnits-1] = u;
}

void unitTarget(struct unit *u, int tx, int ty) {
	if(tileBlocks(tx, ty))
		return;
	u->targetMode = 1;
	u->targetX = tx;
	u->targetY = ty;
}

struct unit *unitAt(int x, int y) {
	for(int i = 0; i < numUnits; i++)
		if(units[i].x == x && units[i].y == y)
			return &units[i];
	return NULL;
}

void moveUnit(struct unit *u, int xm, int ym) {
	if(u->x != u->px || u->y != u->py)
		return;
	if(!xm && !ym)
		return;

	int dx = u->x+xm, dy = u->y+ym;
	if(tileBlocks(dx, dy))
		return;
	if(unitAt(dx, dy))
		return;

	u->x = dx;
	u->y = dy;
	u->progress = 0;
	const int dirs[] = {
		0, -1,
		1, -1,
		1, 0,
		1, 1,
		0, 1,
		-1, 1,
		-1, 0,
		-1, -1,
	};
	for(int i = 0; i < 8; i++) {
		if(xm == dirs[i*2] && ym == dirs[i*2+1])
			u->d = i;
	}
}

void navTo(struct unit *u, int tx, int ty) {
	if(u->x != u->px || u->y != u->py)
		return;

	int *pmap = generatePathmap(u->x, u->y, tx, ty);
	if(!pmap)
		return;

	int xm, ym;
	int t = pmap[u->y*mapW+u->x];
	const int dirs[] = {
		0, -1,
		1, 0,
		0, 1,
		-1, 0,
		1, -1,
		1, 1,
		-1, 1,
		-1, -1,
		0, 0,
	};

	for(int i = 0; i < 9; i++) {
		xm = dirs[i*2];
		ym = dirs[i*2+1];
		if(getTile(pmap, u->x+xm, u->y+ym) == t-1)
			break;
	}

	free(pmap);

	moveUnit(u, xm, ym);
}

void updateUnits(int diff) {
	for(int i = 0; i < numUnits; i++) {
		struct unit *u = &units[i];
		if(u->x != u->px || u->y != u->py) {
			u->progress += unitSpeed(u->type)*diff;
			if(u->progress >= 1.0) {
				u->progress = 0;
				u->px = u->x;
				u->py = u->y;
				u->pd = u->d;
			}
		}
		else if(u->targetMode == 1) {
			if(u->px == u->targetX && u->py == u->targetY)
				u->targetMode = 0;
			else
				navTo(u, u->targetX, u->targetY);
		}
	}
}

SDL_Rect unitDestRect(struct unit *u, int x, int y) {
	int xd = u->x-u->px, yd = u->y-u->py;
	SDL_Rect dst = (SDL_Rect){
		u->px*16+xd*u->progress*16+x,
		u->py*16+yd*u->progress*16+y,
		16, 16
	};

	return dst;
}

void drawUnits(int x, int y) {
	for(int i = 0; i < numUnits; i++) {
		struct unit *u = &units[i];

		float pa = u->pd*45;
		float ca = u->d*45;
		float ad = ca-pa;
		if(ad > 180)
			ad -= 360;
		if(ad < -180)
			ad += 360;
		float am = u->progress*2;
		if(am > 1.0) am = 1;
		float a = pa+ad*am;

		SDL_Rect src = (SDL_Rect){0, u->type*16, 16, 16};

		if(u->px != u->x || u->py != u->y) {
			int s = (int)(u->progress*16)%4;
			if(s%2)
				src.x = 16 + ((s/2)%2)*16;
		}

		int xd = u->x-u->px, yd = u->y-u->py;
		SDL_Rect dst = unitDestRect(u, x, y);
		SDL_Point p = (SDL_Point){8, 8};
		SDL_RenderCopyEx(renderer, unitTex, &src, &dst,
				a, &p, SDL_FLIP_NONE);
	}
}

void drawBottomUnitUI(struct unit *u, int x, int y) {
	if(!u->targetMode)
		return;

	SDL_Rect dst1 = unitDestRect(u, x, y);
	SDL_Rect dst2 = (SDL_Rect){u->targetX*16+x, u->targetY*16+y, 16, 16};

	/* these two don't play nice and I have no idea why >:( */
	/*                   vvvvvvv        */

	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x88);
	//SDL_RenderDrawLine(renderer, dst1.x+8, dst1.y+8, dst2.x+8, dst2.y+8);
	drawLine(dst1.x+8, dst1.y+8, dst2.x+8, dst2.y+8);

	SDL_Rect src = (SDL_Rect){80+32, 0, 16, 16};
	SDL_RenderCopy(renderer, uiTex, &src, &dst2);
}

void drawTopUnitUI(struct unit *u, int x, int y) {
	SDL_Rect src = (SDL_Rect){96, 0, 16, 16};
	SDL_Rect dst = unitDestRect(u, x, y);
	SDL_RenderCopy(renderer, uiTex, &src, &dst);
}

void freeUnits() {
	if(units) {
		free(units);
		units = 0;
		numUnits = 0;
	}
}
