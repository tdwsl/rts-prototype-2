#include <SDL2/SDL.h>
#include <assert.h>
#include "display.h"

#define WPARMS "Stipula RTS",\
	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,\
	640, 480, SDL_WINDOW_RESIZABLE
#define RPARMS win, -1, SDL_RENDERER_ACCELERATED

SDL_Renderer *renderer = NULL;
SDL_Window *win = NULL;

SDL_Texture *scrbuf = NULL;

void initDisplay() {
	assert(SDL_Init(SDL_INIT_EVERYTHING) >= 0);
	assert(win = SDL_CreateWindow(WPARMS));
	assert(renderer = SDL_CreateRenderer(RPARMS));
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	scrbuf = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			320, 240);
	SDL_SetRenderTarget(renderer, scrbuf);

	SDL_ShowCursor(SDL_DISABLE);
}

void endDisplay() {
	SDL_DestroyTexture(scrbuf);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

SDL_Rect getScreenRect() {
	int w, h;
	SDL_GetWindowSize(win, &w, &h);
	float xs = (float)w/320.0, ys = (float)h/240.0;
	float scale = (xs > ys) ? ys : xs;

	SDL_Rect dst = (SDL_Rect){
		w/2-320.0*scale/2, h/2-240.0*scale/2, 320*scale, 240*scale};
	return dst;
}

void updateDisplay() {
	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(renderer);

	SDL_Rect dst = getScreenRect();
	SDL_RenderCopy(renderer, scrbuf, NULL, &dst);
	
	SDL_RenderPresent(renderer);
	SDL_SetRenderTarget(renderer, scrbuf);
}

Uint32 getMouseState(int *x, int *y) {
	Uint32 btn = SDL_GetMouseState(x, y);
	SDL_Rect r = getScreenRect();
	*x -= r.x;
	*y -= r.y;
	*x /= r.w/320.0;
	*y /= r.h/240.0;
	return btn;
}

void drawLine(int x1, int y1, int x2, int y2) {
	int xd = x2-x1, yd = y2-y1;
	float md = (xd*xd > yd*yd) ? xd : yd;
	if(md < 0)
		md *= -1;
	float inc = (1.0/(md+1.0));
	for(float m = 0; m < 1.0; m += inc)
		SDL_RenderDrawPoint(renderer, x1+m*(float)xd,
			y1+m*(float)yd);
}
