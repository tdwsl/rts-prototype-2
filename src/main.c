#include <SDL2/SDL.h>
#include "display.h"
#include "textures.h"
#include "game.h"

int main() {
	initDisplay();
	loadTextures();

	runGame("lvl/0.txt");

	freeTextures();
	endDisplay();
	return 0;
}
