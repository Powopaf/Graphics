#include <SDL2/SDL.h>

char keyQ() {
    return 'f';
}

char key1() {
    return 'c';
}

void findKey(Sint32 key, char* running) {
   if (key == SDLK_q) {
        *running = keyQ();
   }
   if (key == SDLK_AMPERSAND) {
      *running = key1(); 
   }
}
