#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

extern SDL_Renderer *renderer;
extern SDL_Window *win;

void initDisplay();
void endDisplay();
void updateDisplay();
Uint32 getMouseState(int *x, int *y);
void drawLine(int x1, int y1, int x2, int y2);

#endif
