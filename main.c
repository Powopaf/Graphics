#include <stdlib.h>
#include <SDL2/SDL.h>
#include "utils/utils.h"
#include "input/input.h"
#include "shape/shape.h"

int main() {
    if (initSDL("Test", 1500, 700) != 0) {
        return EXIT_FAILURE;
    }
    if (initGL() != 0) {
        return EXIT_FAILURE;
    }
    SDL_Window* window = getSDLWindow();

    // *** MODIFIED ***
    // Before: char running = 't';
    // Now: Start with no shape selected (so everything stays black until a key is pressed).
    char running = 0;

    blackScreen();
    init_cube();
    init_triangle();

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

        // Always clear the screen each frame:
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Only draw if running == 'c' or 't'; otherwise remain black:
        switch (running) {
            case 'c':
                draw_cube();
                break;
            case 't':
                draw_triangle();
                break;
            default:
                // *** MODIFIED ***
                // No shape selected: do nothing (stay black)
                break;
        }

        SDL_GL_SwapWindow(window);
    }

    closeGL();
    closeSDL();
    return EXIT_SUCCESS;
}

