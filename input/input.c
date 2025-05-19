#include <SDL2/SDL.h>
#include "../shape/shape.h"

SDL_bool keyQ() {
    return SDL_FALSE;
}

SDL_bool key1() {
    draw_cube();
    return SDL_TRUE;
}

SDL_bool findKey(Sint32 key, SDL_bool running) {
   if (key == SDLK_q) {
        return keyQ();
   }
   if (key == SDLK_AMPERSAND) {
       return key1();
   }
   return running;
}
