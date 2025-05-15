#include <stdlib.h>
#include <SDL2/SDL.h>
#include "utils/utils.h"
#include "input/input.h"

int main() {
    if (initSDL("Test", 300, 300) != 0) {
        return EXIT_FAILURE;
    }
    if (initGL() != 0) {
        return EXIT_FAILURE;
    }

    SDL_bool running = SDL_TRUE;
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    running = SDL_FALSE;
                    break;  
                case SDL_KEYDOWN:
                    running = findKey(ev.key.keysym.sym, running);
                    break;
                default:
                    break;
            }
        }
        blackScreen();
    }
    return EXIT_SUCCESS;
}
