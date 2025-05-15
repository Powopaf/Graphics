#include <SDL2/SDL.h>

SDL_bool keyQ() {
    return SDL_FALSE;
}

SDL_bool findKey(Sint32 key, SDL_bool running) {
   if (key == SDLK_q) {
        return keyQ();
   }
   return running;
}
