#include <stdlib.h>
#include <stdbool.h>
#include "unit.h"
#include "building.h"
#include "select.h"

struct building **selectedBuildings = 0;
struct unit **selectedUnits = 0;
int numSelectedBuildings = 0;
int numSelectedUnits = 0;

void deselectAll() {
	if(selectedBuildings) {
		free(selectedBuildings);
		selectedBuildings = NULL;
	}
	if(selectedUnits) {
		free(selectedUnits);
		selectedUnits = NULL;
	}
	numSelectedBuildings = 0;
	numSelectedUnits = 0;
}

void selectSingle(int x, int y) {
	struct unit *u = unitAt(x, y);
	if(u) {
		selectedUnits = malloc(sizeof(struct unit*));
		numSelectedUnits = 1;
		selectedUnits[0] = u;
		return;
	}

	struct building *b = buildingAt(x, y);
	if(b) {
		selectedBuildings = malloc(sizeof(struct building*));
		numSelectedBuildings = 1;
		selectedBuildings[0] = b;
	}
}

void selectThrough(int x, int y) {
	for(int i = 0; i < numUnits; i++) {
		struct unit *u = &units[i];
		if(u->x == x && u->y == y) {
			bool dup = false;
			for(int j = 0; j < numSelectedUnits && !dup; j++)
				if(selectedUnits[j] == u)
					dup = true;
			if(dup)
				continue;

			numSelectedUnits++;
			selectedUnits = realloc(selectedUnits,
				sizeof(struct unit*)*numSelectedUnits);
			selectedUnits[numSelectedUnits-1] = u;
		}
	}

	for(int i = 0; i < numBuildings; i++) {
		struct building *b = &buildings[i];
		if(x >= b->x && y >= b->y && x < b->x+b->w && y < b->y+b->h) {
			bool dup = false;
			for(int j = 0; j < numSelectedBuildings && !dup; j++)
				if(selectedBuildings[j] == b)
					dup = true;
			if(dup)
				continue;

			numSelectedBuildings++;
			selectedBuildings = realloc(selectedBuildings,
				sizeof(struct building*)*numSelectedBuildings);
			selectedBuildings[numSelectedBuildings-1] = b;
		}
	}
}
