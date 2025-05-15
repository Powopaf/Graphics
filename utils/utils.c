#include <SDL2/SDL.h>
#include <glad/glad.h>

static SDL_Window* gWindow = NULL;
static SDL_GLContext gGLCtx = NULL;

int initSDL(const char* name, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL init video fail, %s", SDL_GetError());
        return -1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);
    gWindow = SDL_CreateWindow(
            name,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (gWindow) {
        SDL_Log("Error creting window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    return 0;
}

int initGL() {
    if (!gWindow) {
        SDL_Log("initGL call before SDL: %s", SDL_GetError());
        return -1;
    }
    gGLCtx = SDL_GL_CreateContext(gWindow);
    if (!gGLCtx) {
        SDL_Log("SDL create context fail: %s", SDL_GetError());
        gGLCtx = NULL;
        return -1;
    }
    
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_Log("Failed to init glad");
        SDL_GL_DeleteContext(gGLCtx);
        gGLCtx = NULL;
        return -1;
    }
    int w, h;
    SDL_GetWindowSize(gWindow, &w, &h);
    glViewport(0, 0, w, h);
    return 0;
}

void blackSreen() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(gWindow);
}

void closeGL() {
    if (gGLCtx) {
        SDL_GL_DeleteContext(gGLCtx);
        gGLCtx = NULL;
    }
}

void closeSDL() {
    if (gWindow) {
        SDL_DestroyWindow(gWindow);
        gWindow = NULL;
    }
    SDL_Quit();
}
