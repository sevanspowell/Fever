/*===-- app/app.h - Fever 'Hello Triangle' app ----------------------*- C -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 * \brief Fever 'Hello Triangle' application.
 *
 * Based off code provided by 'Alexander Overvoorde' on his 'Vulkan Tutorial'
 * website (https://vulkan-tutorial.com).
 *===----------------------------------------------------------------------===*/
#include <functional>
#include <iostream>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Fever/Fever.h>

/// For automatically freeing Fever resources
template <typename T> class FDeleter {
  public:
    FDeleter() : FDeleter([](T) {}) {}

    FDeleter(std::function<void(T)> deleterFunc) {
        // '=' for capture by value
        deleter = [=](T obj) { deleterFunc(obj); };
    }

    ~FDeleter() { cleanup(); }

    /// Get const reference to object
    const T *operator&() const { return &object; }

    /// Cleanup before returning non-const reference to object
    T *replace() {
        cleanup();
        return &object;
    }

    /// Get object by value
    operator T() const { return object; }

    /// Copy assignment operator
    void operator=(T rhs) {
        if (rhs != object) {
            cleanup();
            object = rhs;
        }
    }

    /// Equality operator
    template <typename V> bool operator==(V rhs) { return object == T(rhs); }

  private:
    T object{FV_NULL_HANDLE};
    std::function<void(T)> deleter;

    void cleanup() {
        if (object != FV_NULL_HANDLE) {
            deleter(object);
        }

        object = FV_NULL_HANDLE;
    }
};

class HelloTriangleApplication {
  public:
    HelloTriangleApplication() : window(nullptr) {}

    ~HelloTriangleApplication() {
        // Clean up window if it hasn't already been cleaned up
        if (window != nullptr) {
            SDL_DestroyWindow(window);
        }
    }

    void run() {
        initWindow();
        initFever();
        mainLoop();
    }

  private:
    void initWindow() {
        // Initialize SDL2
        SDL_InitSubSystem(SDL_INIT_VIDEO);

        // Setup window properties
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
        window = SDL_CreateWindow("Test bed", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, 800, 600, flags);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_CreateContext(window);

        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);

        if (!SDL_GetWindowWMInfo(window, &wmInfo)) {
            throw std::runtime_error("SDL2 could not get window wm info.");
        }
    }

    void initFever() {
        if (fvInit() != FV_SUCCESS) {
            throw std::runtime_error("Failed to initialize Fever library.");
        }
    }

    void shutdownFever() { fvShutdown(); }

    void mainLoop() {
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

        window = nullptr;

        shutdownFever();
    }

    SDL_Window *window;
};

int main(void) {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
