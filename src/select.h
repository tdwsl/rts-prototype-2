#ifndef SELECT_H
#define SELECT_H

#include "building.h"
#include "unit.h"

extern struct building **selectedBuildings;
extern struct unit **selectedUnits;
extern int numSelectedBuildings;
extern int numSelectedUnits;

void selectSingle(int x, int y);
void selectThrough(int x, int y);
void deselectAll();

#endif
