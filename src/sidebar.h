#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <SDL2/SDL.h>
#include "unit.h"
#include "building.h"

enum {
	BUTTON_NONE=0,
	BUTTON_NEWUNIT,
	BUTTON_NEWBUILDING,
};

struct sidebarButton {
	int buttonType, type;
	struct building *building;
	struct unit *unit;
};

extern struct sidebarButton sidebarButtons[4];

void rebuildSidebar();
void clickSidebar(Uint32 btn, int mx, int my);
void drawSidebar();

#endif
