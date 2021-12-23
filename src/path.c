#include <stdlib.h>
#include <stdbool.h>
#include "map.h"
#include "unit.h"
#include "path.h"

int *generatePathmap(int x1, int y1, int x2, int y2) {
	if(x1 == x2 && y1 == y2)
		return 0;
	if(tileBlocks(x2, y2))
		return 0;

	int *pmap = malloc(sizeof(int)*mapW*mapH);
	for(int i = 0; i < mapW*mapH; i++)
		pmap[i] = blocks(map[i])*-1;
	for(int i = 0; i < numUnits; i++)
		pmap[units[i].y*mapW+units[i].x] = -1;

	pmap[y2*mapW+x2] = 1;
	pmap[y1*mapW+x1] = 0;

	bool hasUnits = true;
	int i;

	for(i = 1; !pmap[y1*mapW+x1] && i < 999+999; i++) {
		bool stuck = true;

		for(int j = 0; j < mapW*mapH; j++) {
			if(pmap[j] != i)
				continue;
			stuck = false;

			for(int x = -1; x <= 1; x++)
				for(int y = -1; y <= 1; y++)
					tsetTile(pmap, j%mapW+x, j/mapW+y, i+1);
		}

		if(i == 999)
			stuck = true;

		if(stuck) {
			if(!hasUnits)
				break;
			for(int i = 0; i < mapW*mapH; i++)
				if(pmap[i] == -1)
					pmap[i] = blocks(map[i])*-1;
			hasUnits = false;
		}
	}

	if(!pmap[y1*mapW+x1]) {
		free(pmap);
		return 0;
	}

	/*for(int y = 0; y < mapH; y++) {
		for(int x = 0; x < mapW; x++)
			printf("%3d", pmap[y*mapW+x]);
		printf("\n");
	}*/

	return pmap;
}
