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
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Fever/Fever.h>
#include <Fever/FeverPlatform.h>

#if FV_PLATFORM_OSX
#include "osx.h"
#endif

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

        if (window == nullptr) {
            std::stringstream err;
            err << "Could not create window: " << SDL_GetError();
            throw std::runtime_error(err.str());
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
    }

    void initFever() {
        // Create surface
        SDL_SysWMinfo windowInfo;
        SDL_VERSION(&windowInfo.version);
        if (!SDL_GetWindowWMInfo(window, &windowInfo)) {
            throw std::runtime_error("Failed to get SDL2 window info.");
        }

#if FV_PLATFORM_OSX
        if (getOSXSurface(surface.replace(), windowInfo) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to get OSX surface.");
        }
#endif

        // Initialize fever
        FvInitInfo initInfo;
        initInfo.surface = surface;

        if (fvInit(&initInfo) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to initialize Fever library.");
        }

        createGraphicsPipeline();
    }

    void createGraphicsPipeline() {
        std::vector<char> shaderCode =
            readFile("src/projects/app/assets/hello.metal");

        FDeleter<FvShaderModule> shaderModule{fvShaderModuleDestroy};

        FvShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.data = (void *)shaderCode.data();
        shaderModuleCreateInfo.size = shaderCode.size();

        if (fvShaderModuleCreate(shaderModule.replace(),
                                 &shaderModuleCreateInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create shader module!");
        }

        FvPipelineShaderStageDescription vertShaderStageInfo = {};
        vertShaderStageInfo.stage             = FV_SHADER_STAGE_VERTEX;
        vertShaderStageInfo.entryFunctionName = "vertFunc";
        vertShaderStageInfo.shaderModule      = shaderModule;

        FvPipelineShaderStageDescription fragShaderStageInfo = {};
        fragShaderStageInfo.stage             = FV_SHADER_STAGE_FRAGMENT;
        fragShaderStageInfo.entryFunctionName = "fragFunc";
        fragShaderStageInfo.shaderModule      = shaderModule;

        FvPipelineShaderStageDescription shaderStages[] = {vertShaderStageInfo,
                                                           fragShaderStageInfo};
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

    static std::vector<char> readFile(const std::string &filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            std::stringstream err;
            err << "Failed to open file: " << filename << ".";
            throw std::runtime_error(err.str());
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    SDL_Window *window;
    FDeleter<FvSurface> surface{fvDestroySurface};
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
