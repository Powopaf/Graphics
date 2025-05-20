#include <SDL2/SDL.h>

void findKey(Sint32 key, char* running) {
   if (key == SDLK_q) {
        *running = 'f';
   }
   if (key == SDLK_1) {
      *running = 'c';
   }
}
