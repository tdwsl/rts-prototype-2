#include <SDL2/SDL.h>
#include <stdbool.h>
#include "map.h"
#include "display.h"
#include "textures.h"
#include "unit.h"
#include "building.h"
#include "sidebar.h"
#include "select.h"
#include "game.h"

float cameraX=0, cameraY=0, cameraXV=0, cameraYV=0;

Uint32 btn = 0;
int oldX=0, oldY=0;
float oldCamX=0, oldCamY=0;
bool dragging = false;

void update(int diff) {
	cameraX += cameraXV*diff;
	cameraY += cameraYV*diff;

	if(btn) {
		int mx, my;
		getMouseState(&mx, &my);

		int xd = oldX-mx, yd = oldY-my;
		if(xd*xd+yd*yd > 8*8)
			dragging = true;

		if(dragging)
			if(btn & SDL_BUTTON_RMASK) {
				cameraX = oldCamX+xd;
				cameraY = oldCamY+yd;
			}
	}

	if(cameraX+240 > mapW*16)
		cameraX = mapW*16-240;
	if(cameraY+240 > mapH*16)
		cameraY = mapH*16-240;
	if(cameraX < 0)
		cameraX = 0;
	if(cameraY < 0)
		cameraY = 0;

	updateBuildings(diff);
	updateUnits(diff);
}

void drawCursor() {
	int mx, my;
	getMouseState(&mx, &my);
	SDL_Rect src = (SDL_Rect){80, 0, 8, 8};
	SDL_Rect dst = (SDL_Rect){mx, my, 8, 8};
	SDL_RenderCopy(renderer, uiTex, &src, &dst);
}

SDL_Rect selectionRect(int mx, int my) {
	SDL_Rect r = (SDL_Rect){oldX+oldCamX, oldY+oldCamY,
		cameraX-oldCamX+mx-oldX, cameraY-oldCamY+my-oldY};
	if(r.w < 0) {
		r.x += r.w;
		r.w *= -1;
	}
	if(r.h < 0) {
		r.y += r.h;
		r.h *= -1;
	}
	return r;
}

void draw() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(renderer);

	drawMap(-cameraX, -cameraY);

	for(int i = 0; i < numSelectedUnits; i++)
		drawBottomUnitUI(selectedUnits[i], -cameraX, -cameraY);

	drawUnits(-cameraX, -cameraY);

	for(int i = 0; i < numSelectedUnits; i++)
		drawTopUnitUI(selectedUnits[i], -cameraX, -cameraY);

	for(int i = 0; i < numSelectedBuildings; i++)
		drawBuildingUI(selectedBuildings[i], -cameraX, -cameraY);

	int mx, my;
	getMouseState(&mx, &my);

	if(dragging && btn & SDL_BUTTON_LMASK) {
		SDL_Rect r = selectionRect(mx, my);
		r.x -= cameraX;
		r.y -= cameraY;

		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x88);
		SDL_RenderFillRect(renderer, &r);
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderDrawRect(renderer, &r);
	}

	drawSidebar();
	drawCursor();

	updateDisplay();
}

void click() {
	int mx, my;
	getMouseState(&mx, &my);

	if(oldX >= 240) {
		if(!dragging)
			clickSidebar(btn, mx, my);
		return;
	}

	int tx = (mx+cameraX)/16, ty = (my+cameraY)/16;

	if(btn & SDL_BUTTON_LMASK) {
		deselectAll();

		if(dragging) {
			SDL_Rect r = selectionRect(mx, my);

			for(int x = r.x/16; x <= (r.x+r.w)/16; x++)
				for(int y = r.y/16; y <= (r.y+r.h)/16; y++)
					selectThrough(x, y);
		}
		else
			selectSingle(tx, ty);
	}
	else if(btn & SDL_BUTTON_RMASK) {
		for(int i = 0; i < numSelectedUnits; i++)
			unitTarget(selectedUnits[i], tx, ty);
		for(int i = 0; i < numSelectedBuildings; i++)
			buildingTarget(selectedBuildings[i], tx, ty);
	}

	rebuildSidebar();
}

void runGame(const char *filename) {
	loadMap(filename);
	generateBuildings();
	rebuildSidebar();

	bool quit = false;
	int lastUpdate = SDL_GetTicks();

	while(!quit) {
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
			switch(ev.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch(ev.key.keysym.sym) {
				case SDLK_UP: cameraYV = -0.15; break;
				case SDLK_DOWN: cameraYV = 0.15; break;
				case SDLK_LEFT: cameraXV = -0.15; break;
				case SDLK_RIGHT: cameraXV = 0.15; break;
				}
				break;
			case SDL_KEYUP:
				switch(ev.key.keysym.sym) {
				case SDLK_UP:
					if(cameraYV < 0) cameraYV = 0;
					break;
				case SDLK_DOWN:
					if(cameraYV > 0) cameraYV = 0;
					break;
				case SDLK_LEFT:
					if(cameraXV < 0) cameraXV = 0;
					break;
				case SDLK_RIGHT:
					if(cameraXV > 0) cameraXV = 0;
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				oldCamX = cameraX;
				oldCamY = cameraY;
				btn = getMouseState(&oldX, &oldY);
				dragging = false;
				break;
			case SDL_MOUSEBUTTONUP:
				click();
				btn = 0;
			}

		int currentTime = SDL_GetTicks();
		int diff = currentTime-lastUpdate;
		lastUpdate = currentTime;

		update(diff);

		draw();
	}

	deselectAll();
	freeUnits();
	freeBuildings();
	free(map);
}
