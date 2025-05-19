#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>

SDL_Window* getSDLWindow();
int initSDL(const char* name, int width, int height);
int initGL();
void blackScreen();
void closeGL();
void closeSDL();

#endif
