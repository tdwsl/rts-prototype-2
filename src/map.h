#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

extern int *map, mapW, mapH;

void loadMap(const char *filename);
int getTile(int *map, int x, int y);
void setTile(int *map, int x, int y, int t);
void tsetTile(int *map, int x, int y, int t);
void drawMap(int x, int y);
bool tileBlocks(int x, int y);
bool blocks(int t);

#endif
