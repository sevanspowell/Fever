/*===-- app/app.h - Fever 'Hello Triangle' app --------------------*- C++ -*-===
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
    HelloTriangleApplication(SDL_Window *windowPtr)
        : window(windowPtr), commandBuffer(FV_NULL_HANDLE) {}

    ~HelloTriangleApplication() {
        // Clean up window if it hasn't already been cleaned up
        if (window != nullptr) {
            SDL_DestroyWindow(window);
        }
    }

    void run() {
        // initWindow();
        initFever();
        mainLoop();
    }

  private:
    void initWindow() {}

    void initFever() {
        createRenderPass();
        createSwapchain();
        createGraphicsPipeline();
        createSemaphores();
    }

    void createRenderPass() {
        FvAttachmentDescription colorAttachment = {};
        colorAttachment.format                  = FV_FORMAT_BGRA8UNORM;
        colorAttachment.samples                 = FV_SAMPLE_COUNT_1;
        colorAttachment.loadOp                  = FV_LOAD_OP_CLEAR;
        colorAttachment.storeOp                 = FV_STORE_OP_STORE;
        colorAttachment.stencilLoadOp           = FV_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp          = FV_STORE_OP_DONT_CARE;

        FvAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment            = 0;

        FvSubpassDescription subpass = {};
        subpass.inputAttachmentCount = 0;
        subpass.inputAttachments     = nullptr;
        subpass.colorAttachmentCount = 1;
        subpass.colorAttachments     = &colorAttachmentRef;

        FvRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.attachmentCount        = 1;
        renderPassInfo.attachments            = &colorAttachment;
        renderPassInfo.subpassCount           = 1;
        renderPassInfo.subpasses              = &subpass;

        if (fvRenderPassCreate(renderPass.replace(), &renderPassInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create render pass!");
        }
    }

    void createSwapchain() {
        int outputWidth = 0;
        int outputHeight = 0;
        
        SDL_GL_GetDrawableSize(window, &outputWidth, &outputHeight);
        
        FvSwapchain oldSwapchain = swapchain;

        FvSwapchainCreateInfo swapchainCreateInfo;
        swapchainCreateInfo.oldSwapchain = oldSwapchain;
        swapchainCreateInfo.extent.width = outputWidth;
        swapchainCreateInfo.extent.height = outputHeight;


        FvSwapchain newSwapchain;
        if (fvCreateSwapchain(&newSwapchain, &swapchainCreateInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create swapchain!");
        }

        swapchain = newSwapchain;

        fvGetSwapchainImage(swapchain, &swapchainImage);
    }

    void recreateSwapchain() {
        fvDeviceWaitIdle();

        createSwapchain();
        createRenderPass();
        createGraphicsPipeline();
    }

    void createGraphicsPipeline() {
        int outputWidth = 0;
        int outputHeight = 0;

        SDL_GL_GetDrawableSize(window, &outputWidth, &outputHeight);

        std::vector<char> shaderCode =

            readFile("src/projects/app/assets/hello.metal");
        shaderCode.push_back('\0');

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

        FvPipelineVertexInputDescription vertexInputInfo = {};
        vertexInputInfo.vertexBindingDescriptionCount    = 0;
        vertexInputInfo.vertexBindingDescriptions        = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount  = 0;
        vertexInputInfo.vertexAttributeDescriptions      = nullptr;

        FvPipelineInputAssemblyDescription inputAssembly = {};
        inputAssembly.primitiveType          = FV_PRIMITIVE_TYPE_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = true;

        FvViewport viewport = {};
        viewport.x          = 0.0f;
        viewport.y          = 0.0f;
        viewport.width      = (float)outputWidth;
        viewport.height     = (float)outputHeight;
        viewport.minDepth   = 0.0f;
        viewport.maxDepth   = 1.0f;

        FvRect2D scissor = {};
        scissor.offset   = {0, 0};
        scissor.extent.width = (uint32_t)outputWidth;
        scissor.extent.height = (uint32_t)outputHeight;

        FvPipelineViewportDescription viewportState = {};
        viewportState.viewport                      = viewport;
        viewportState.scissor                       = scissor;

        FvPipelineRasterizerDescription rasterizer = {};
        rasterizer.depthClampEnable                = false;
        /* rasterizer.rasterizerDiscardEnable         = false; */
        rasterizer.cullMode    = FV_CULL_MODE_BACK;
        rasterizer.frontFacing = FV_WINDING_ORDER_COUNTER_CLOCKWISE;

        FvColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.blendEnable                 = false;
        colorBlendAttachment.colorWriteMask = (FvColorComponentFlags)(
            FV_COLOR_COMPONENT_R | FV_COLOR_COMPONENT_G | FV_COLOR_COMPONENT_B |
            FV_COLOR_COMPONENT_A);

        FvPipelineColorBlendStateDescription colorBlending = {};
        colorBlending.attachmentCount                      = 1;
        colorBlending.attachments = &colorBlendAttachment;

        FvPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.setLayoutCount             = 0;
        pipelineLayoutInfo.pushConstantRangeCount     = 0;

        if (fvPipelineLayoutCreate(pipelineLayout.replace(),
                                   &pipelineLayoutInfo) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        FvGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.stageCount                   = 2;
        pipelineInfo.stages                       = shaderStages;
        pipelineInfo.vertexInputDescription       = &vertexInputInfo;
        pipelineInfo.inputAssemblyDescription     = &inputAssembly;
        pipelineInfo.viewportDescription          = &viewportState;
        pipelineInfo.rasterizerDescription        = &rasterizer;
        pipelineInfo.colorBlendStateDescription   = &colorBlending;
        pipelineInfo.depthStencilDescription      = nullptr;
        pipelineInfo.layout                       = pipelineLayout;
        pipelineInfo.renderPass                   = renderPass;
        pipelineInfo.subpass                      = 0;

        if (fvGraphicsPipelineCreate(graphicsPipeline.replace(),
                                     &pipelineInfo) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }

        /* FvImageCreateInfo imageInfo = {}; */
        /* imageInfo.format            = FV_FORMAT_BGRA8UNORM; */
        /* imageInfo.imageType         = FV_IMAGE_TYPE_2D; */
        /* imageInfo.extent.width      = 800; */
        /* imageInfo.extent.height     = 600; */
        /* imageInfo.extent.depth      = 1; */
        /* imageInfo.numMipmapLevels   = 1; */
        /* imageInfo.arrayLayers       = 1; */
        /* imageInfo.numSamples        = FV_SAMPLE_COUNT_1; */
        /* imageInfo.usage             = FV_IMAGE_USAGE_RENDER_TARGET; */

        /* FvImage image; */
        /* if (fvImageCreate(&image, &imageInfo) != FV_RESULT_SUCCESS) { */
        /*     throw std::runtime_error("Failed to create image!"); */
        /* } */

        FvImageViewCreateInfo imageViewInfo{};
        imageViewInfo.image    = swapchainImage;
        imageViewInfo.viewType = FV_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format   = FV_FORMAT_BGRA8UNORM;

        FvImageView imageView;
        if (fvImageViewCreate(&imageView, &imageViewInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create image view!");
        }

        FvFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.renderPass              = renderPass;
        framebufferInfo.attachmentCount         = 1;
        framebufferInfo.attachments             = &imageView;
        framebufferInfo.width                   = outputWidth;
        framebufferInfo.height                  = outputHeight;
        framebufferInfo.layers                  = 1;

        if (fvFramebufferCreate(framebuffer.replace(), &framebufferInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }

        FvCommandPoolCreateInfo poolInfo = {};

        if (fvCommandPoolCreate(commandPool.replace(), &poolInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }

        // If a command buffer already exists, free it
        if (commandBuffer != FV_NULL_HANDLE) {
            fvCommandBufferDestroy(commandBuffer, commandPool);
        }

        if (fvCommandBufferCreate(&commandBuffer, commandPool) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create command buffer!");
        }

        fvCommandBufferBegin(commandBuffer);

        FvRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.renderPass            = renderPass;
        renderPassInfo.framebuffer           = framebuffer;

        FvClearValue clearColor        = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.clearValues     = &clearColor;

        fvCmdBeginRenderPass(commandBuffer, &renderPassInfo);

        {
            fvCmdBindGraphicsPipeline(commandBuffer, graphicsPipeline);

            fvCmdDraw(commandBuffer, 3, 1, 0, 0);
        }

        fvCmdEndRenderPass(commandBuffer);

        if (fvCommandBufferEnd(commandBuffer) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer");
        }
    }

    void createSemaphores() {
        if (fvSemaphoreCreate(imageAvailableSemaphore.replace()) !=
                FV_RESULT_SUCCESS ||
            fvSemaphoreCreate(renderFinishedSemaphore.replace()) !=
                FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create semaphores!");
        }
    }

    void drawFrame() {
        uint32_t imageIndex;
        if (fvAcquireNextImage(swapchain, imageAvailableSemaphore,
                               &imageIndex) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to acquire image!");
        }

        FvSubmitInfo submitInfo;

        FvSemaphore waitSemaphores[]  = {imageAvailableSemaphore};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.waitSemaphores     = waitSemaphores;

        submitInfo.commandBufferCount = 1;
        submitInfo.commandBuffers     = &commandBuffer;

        FvSemaphore signalSemaphores[]  = {renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.signalSemaphores     = signalSemaphores;

        if (fvQueueSubmit(1, &submitInfo) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer!");
        }

        FvPresentInfo presentInfo;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.waitSemaphores     = signalSemaphores;
        presentInfo.swapchainCount     = 1;
        FvSwapchain swapchains[]       = {swapchain};
        presentInfo.swapchains         = swapchains;
        presentInfo.imageIndices       = &imageIndex;

        fvQueuePresent(&presentInfo);
    }

    void mainLoop() {
        SDL_Event event;

        // Grab events from platform
        bool shouldQuit = false;
        while (!shouldQuit) {
            while (SDL_PollEvent(&event) && !shouldQuit) {
                switch (event.type) {
                case SDL_QUIT: {
                    shouldQuit = true;
                    break;
                }
                case SDL_WINDOWEVENT: {
                    switch (event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        recreateSwapchain();
                        break;
                    }
                    break;
                }
                }
            }

            drawFrame();
        }

        window = nullptr;

        fvDeviceWaitIdle();
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
    FDeleter<FvPipelineLayout> pipelineLayout{fvPipelineLayoutDestroy};
    FDeleter<FvRenderPass> renderPass{fvRenderPassDestroy};
    FDeleter<FvGraphicsPipeline> graphicsPipeline{fvGraphicsPipelineDestroy};
    FDeleter<FvFramebuffer> framebuffer{fvFramebufferDestroy};
    FDeleter<FvCommandPool> commandPool{fvCommandPoolDestroy};
    // Resources of command buffer automatically freed
    FvCommandBuffer commandBuffer;
    FDeleter<FvSwapchain> swapchain{fvDestroySwapchain};
    FvImage swapchainImage;
    FDeleter<FvSemaphore> imageAvailableSemaphore{fvSemaphoreDestroy};
    FDeleter<FvSemaphore> renderFinishedSemaphore{fvSemaphoreDestroy};
};

int main(void) {

    try {
        // Initialize SDL2
        SDL_Init(SDL_INIT_VIDEO);

        uint32_t flags = 0x0;
        flags |= SDL_WINDOW_RESIZABLE;

        // Create window
        SDL_Window *window =
            SDL_CreateWindow("Test bed", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, 800, 600, flags);

        if (window == nullptr) {
            std::stringstream err;
            err << "Could not create window: " << SDL_GetError();
            throw std::runtime_error(err.str());
        }

        // Create surface
        FDeleter<FvSurface> surface{fvDestroySurface};

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
        while (true) {
            HelloTriangleApplication app(window);

            app.run();
        }

        fvShutdown();

        SDL_DestroyWindow(window);
        SDL_Quit();
    } catch (const std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
