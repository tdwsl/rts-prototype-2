#include <SDL2/SDL.h>
#include <stdbool.h>
#include "display.h"
#include "textures.h"
#include "select.h"
#include "drawText.h"
#include "unit.h"
#include "building.h"
#include "sidebar.h"

struct sidebarButton sidebarButtons[4];
int sidebarScroll=0;
bool scrollDown=false;

void rebuildSidebar() {
	struct sidebarButton *buttons=0;
	int sz = 0;

	for(int i = 0; i < numSelectedBuildings; i++) {
		struct building *b = selectedBuildings[i];
		const int *units = buildingUnits(b->type);
		for(int i = 0; units[i] != -1; i++) {
			sz++;
			buttons = realloc(buttons,
				sizeof(struct sidebarButton)*sz);
			buttons[sz-1].buttonType = BUTTON_NEWUNIT;
			buttons[sz-1].building = b;
			buttons[sz-1].unit = NULL;
			buttons[sz-1].type = units[i];
		}
	}

	if(sidebarScroll > sz-4)
		sidebarScroll = sz-4;
	if(sidebarScroll < 0)
		sidebarScroll = 0;

	for(int i = 0; i < 4; i++) {
		sidebarButtons[i].buttonType = BUTTON_NONE;
	}

	for(int i = sidebarScroll; i < sz && i < sidebarScroll+4; i++)
		sidebarButtons[i-sidebarScroll] = buttons[i];

	scrollDown = (sidebarScroll+4 < sz);

	if(buttons)
		free(buttons);
}

void clickSidebar(Uint32 btn, int mx, int my) {
	mx -= 240;
	
	if(mx >= 8 && my >= 128 && mx < 72 && my < 224) {
		int i = (my-128)/24;
		struct sidebarButton *b = &sidebarButtons[i];
		if(sidebarButtons[i].buttonType == BUTTON_NEWUNIT) {
			if(btn & SDL_BUTTON_LMASK)
				if(b->type != b->building->producing) {
					b->building->producing = b->type;
					b->building->progress = 0;
				}
			if(btn & SDL_BUTTON_RMASK)
				if(b->type == b->building->producing)
					b->building->producing = -1;
		}
	}

	if(mx >= 8 && my >= 224 && mx < 72 && my < 224+8) {
		sidebarScroll += ((mx-8)/32)*2 - 1;
		rebuildSidebar();
	}
}

void drawSidebar() {
	SDL_Rect src = (SDL_Rect){0, 0, 80, 240};
	SDL_Rect dst = (SDL_Rect){240, 0, 80, 240};
	SDL_RenderCopy(renderer, uiTex, &src, &dst);

	for(int i = 0; i < 4 &&
			sidebarButtons[i].buttonType != BUTTON_NONE; i++) {
		src = (SDL_Rect){80, 16, 64, 24};
		dst = (SDL_Rect){240+8, 128+24*i, 64, 24};
		SDL_RenderCopy(renderer, uiTex, &src, &dst);

		struct sidebarButton *b = &sidebarButtons[i];

		if(b->buttonType == BUTTON_NEWUNIT) {
			if(b->type == b->building->producing) {
				dst.w *= b->building->progress;
				SDL_SetRenderDrawColor(renderer,
					0xff, 0xff, 0xff, 0x44);
				SDL_RenderFillRect(renderer, &dst);
			}

			src = (SDL_Rect){0, b->type*16, 16, 16};
			dst = (SDL_Rect){240+8+4, 128+24*i+4, 16, 16};
			SDL_RenderCopy(renderer, unitTex, &src, &dst);
		}

		const char *text;
		if(b->buttonType == BUTTON_NEWUNIT)
			text = unitName(b->type);
		else if(b->buttonType == BUTTON_NEWBUILDING)
			text = buildingName(b->type);
		drawText(240+8+24, 128+24*i+2, text);
	}

	src = (SDL_Rect){80, 16+24, 32, 8};
	dst = (SDL_Rect){240+8, 224, 32, 8};

	if(sidebarScroll > 0)
		SDL_RenderCopy(renderer, uiTex, &src, &dst);

	if(scrollDown) {
		src.x += 32;
		dst.x += 32;
		SDL_RenderCopy(renderer, uiTex, &src, &dst);
	}
}
