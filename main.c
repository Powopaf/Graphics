#include <stdlib.h>
#include <SDL2/SDL.h>
#include "utils/utils.h"
#include "input/input.h"
#include "shape/shape.h"

int main() {
    if (initSDL("Test", 300, 300) != 0) {
        return EXIT_FAILURE;
    }
    if (initGL() != 0) {
        return EXIT_FAILURE;
    }

    char running = 't';
    blackScreen();
    init_cube();
    while (running != 'f') {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    running = 'f';
                    break;  
                case SDL_KEYDOWN:
                        findKey(ev.key.keysym.sym, &running);
                    break;
                default:
                    break;
            }
        }
        switch (running) {
            case 'c':
                draw_cube();
        }
    }
    return EXIT_SUCCESS;
}
