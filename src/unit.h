#ifndef UNIT_H
#define UNIT_H

enum {
	UNITTYPE_SOLDIER=0,
	UNITTYPE_TANK,
	UNITTYPE_TRUCK,
};

struct unit {
	int x, y, px, py;
	float progress;
	int type, team;
	int hp;
	int d, pd;
	int targetX, targetY;
	int targetMode;
	struct unit *targetUnit;
};

extern struct unit *units;
extern int numUnits;

struct unit *unitAt(int x, int y);
void drawUnits(int x, int y);
void addUnit(int x, int y, int type, int team);
void updateUnits(int diff);
void freeUnits();
const char *unitName(int type);
void unitTarget(struct unit *u, int tx, int ty);
void drawTopUnitUI(struct unit *u, int x, int y);
void drawBottomUnitUI(struct unit *u, int x, int y);
float unitProductionSpeed(int type);

#endif
