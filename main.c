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
    SDL_Window* window = getSDLWindow();
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        switch (running) {
            case 'c':
                draw_cube();
                break;
            default:
                break;
        }
        SDL_GL_SwapWindow(window);
    }
    return EXIT_SUCCESS;
}
