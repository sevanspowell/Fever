#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <Fever/Sample.h>

int main(void) {
    SDL_InitSubSystem(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    uint32_t flags = 0x0;
    flags |= SDL_WINDOW_OPENGL;
    flags |= SDL_WINDOW_RESIZABLE;

    // Create window
    SDL_Window *window =
        SDL_CreateWindow("Test bed", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 800, 600, flags);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_CreateContext(window);

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if (!SDL_GetWindowWMInfo(window, &wmInfo)) {
        fprintf(stderr, "render: SDL2 could not get wm info");
        return EXIT_FAILURE;
    }

    SDL_Event event;

    // Grab events from platform
    int shouldQuit = 0;
    while (!shouldQuit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT: {
                shouldQuit = 1;
                break;
            }
            }
        }

        // Swap display buffers
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);

    printf("Get three: %d\n", fvr_sample_getThree());
    printf("Get four: %d\n", fvr_sample_getFour(NULL));

    return 0;
}
