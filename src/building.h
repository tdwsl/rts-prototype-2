#ifndef BUILDING_H
#define BUILDING_H

enum {
	BTYPE_POWERPLANT=0,
	BTYPE_REFINERY,
	BTYPE_MAX,
};

struct building {
	int x, y, w, h;
	int spawnX, spawnY;
	int targetX, targetY;
	float progress;
	int team, type;
	int producing;
	int hp;
};

extern struct building *buildings;
extern int numBuildings;

struct building *buildingAt(int x, int y);
void addBuilding(int x, int y, int type, int team);
void generateBuildings();
void freeBuildings();
void updateBuildings(int diff);
const char *buildingName(int type);
void drawBuildingUI(struct building *b, int x, int y);
const int *buildingUnits(int type);
int buildingTile(int type);
void buildingTarget(struct building *b, int tx, int ty);

#endif
