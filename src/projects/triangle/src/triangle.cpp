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
#include <array>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Fever/Fever.h>
#include <Fever/FeverPlatform.h>
#include <Fever/FeverSurfaceAcquisition.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    static FvVertexInputBindingDescription getBindingDescription() {
        FvVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding                         = 0;
        bindingDescription.stride                          = sizeof(Vertex);
        bindingDescription.inputRate = FV_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<FvVertexInputAttributeDescription, 2>
    getAttributeDescriptions() {
        std::array<FvVertexInputAttributeDescription, 2> attributeDescriptions =
            {};

        attributeDescriptions[0].binding  = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format   = FV_VERTEX_FORMAT_FLOAT3;
        attributeDescriptions[0].offset   = offsetof(Vertex, pos);

        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format   = FV_VERTEX_FORMAT_FLOAT3;
        attributeDescriptions[1].offset   = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

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
        : window(windowPtr), outputWidth(0), outputHeight(0),
          commandBuffer(FV_NULL_HANDLE) {}

    ~HelloTriangleApplication() {
        // Clean up window if it hasn't already been cleaned up
        if (window != nullptr) {
            SDL_DestroyWindow(window);
        }
    }

    void run() {
        SDL_GL_GetDrawableSize(window, &outputWidth, &outputHeight);

        initFever();
        mainLoop();
    }

  private:
    void initFever() {
        createSwapchain();
        createRenderPass();
        createDescriptorSet();
        createGraphicsPipeline();
        createCommandPool();
        createDepthResources();
        createFramebuffer();
        prepareVertices();
        prepareIndices();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffer();
        writeDescriptorSet();
        createCommandBuffer();
        createSemaphores();
    }

    void createSwapchain() {
        FvSwapchain oldSwapchain = swapchain;

        FvSwapchainCreateInfo swapchainCreateInfo;
        swapchainCreateInfo.oldSwapchain  = oldSwapchain;
        swapchainCreateInfo.extent.width  = outputWidth;
        swapchainCreateInfo.extent.height = outputHeight;

        FvSwapchain newSwapchain;
        if (fvCreateSwapchain(&newSwapchain, &swapchainCreateInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create swapchain!");
        }

        swapchain = newSwapchain;

        fvGetSwapchainImage(swapchain, &swapchainImage);
    }

    void createRenderPass() {
        FvAttachmentDescription colorAttachment = {};
        colorAttachment.format                  = FV_FORMAT_BGRA8UNORM;
        colorAttachment.samples                 = FV_SAMPLE_COUNT_1;
        colorAttachment.loadOp                  = FV_LOAD_OP_CLEAR;
        colorAttachment.storeOp                 = FV_STORE_OP_STORE;
        colorAttachment.stencilLoadOp           = FV_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp          = FV_STORE_OP_DONT_CARE;

        FvAttachmentDescription depthAttachment = {};
        depthAttachment.format                  = FV_FORMAT_DEPTH32FLOAT;
        depthAttachment.samples                 = FV_SAMPLE_COUNT_1;
        depthAttachment.loadOp                  = FV_LOAD_OP_CLEAR;
        depthAttachment.storeOp                 = FV_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp           = FV_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp          = FV_STORE_OP_DONT_CARE;

        FvAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment            = 0;

        FvAttachmentReference depthAttachmentRef = {};
        depthAttachmentRef.attachment            = 1;

        FvSubpassDescription subpass   = {};
        subpass.inputAttachmentCount   = 0;
        subpass.inputAttachments       = nullptr;
        subpass.colorAttachmentCount   = 1;
        subpass.colorAttachments       = &colorAttachmentRef;
        subpass.depthStencilAttachment = &depthAttachmentRef;

        FvSubpassDependency dependency = {};
        dependency.srcSubpass          = FV_SUBPASS_EXTERNAL;
        dependency.dstSubpass          = 0;
        dependency.srcStageMask  = FV_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask  = FV_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
        dependency.dstAccessMask = FV_ACCESS_FLAGS_COLOR_ATTACHMENT_READ |
                                   FV_ACCESS_FLAGS_COLOR_ATTACHMENT_WRITE;

        std::array<FvAttachmentDescription, 2> attachments = {colorAttachment,
                                                              depthAttachment};

        FvRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.attachmentCount        = attachments.size();
        renderPassInfo.attachments            = attachments.data();
        renderPassInfo.subpassCount           = 1;
        renderPassInfo.subpasses              = &subpass;
        renderPassInfo.dependencyCount        = 1;
        renderPassInfo.dependencies           = &dependency;

        if (fvRenderPassCreate(renderPass.replace(), &renderPassInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create render pass!");
        }
    }

    void recreateSwapchain() {
        fvDeviceWaitIdle();

        createSwapchain();
        createDepthResources();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffer();
        createCommandBuffer();
    }

    void prepareVertices() {
        vertices = {{{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                    {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                    {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}};
    }

    void prepareIndices() { indices = {2, 1, 0}; }

    void createVertexBuffer() {
        FvBufferCreateInfo bufferInfo = {};
        bufferInfo.size               = sizeof(vertices[0]) * vertices.size();
        bufferInfo.usage              = FV_BUFFER_USAGE_VERTEX_BUFFER;
        bufferInfo.data               = vertices.data();

        if (fvBufferCreate(vertexBuffer.replace(), &bufferInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create vertex buffer!");
        }
    }

    void createIndexBuffer() {
        FvBufferCreateInfo bufferInfo = {};
        bufferInfo.size               = sizeof(indices[0]) * indices.size();
        bufferInfo.usage              = FV_BUFFER_USAGE_INDEX_BUFFER;
        bufferInfo.data               = indices.data();

        if (fvBufferCreate(indexBuffer.replace(), &bufferInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create vertex buffer!");
        }
    }

    void createUniformBuffer() {
        FvBufferCreateInfo bufferInfo = {};
        bufferInfo.size               = sizeof(UniformBufferObject);
        bufferInfo.usage              = FV_BUFFER_USAGE_INDEX_BUFFER;
        bufferInfo.data = nullptr; // Add data in updateUniformBuffer

        if (fvBufferCreate(uniformBuffer.replace(), &bufferInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create vertex buffer!");
        }
    }

    void writeDescriptorSet() {
        FvDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer                 = uniformBuffer;
        bufferInfo.offset                 = 0;
        bufferInfo.range                  = sizeof(UniformBufferObject);

        // Get shader reflection information
        FvShaderReflectionRequest uniformBufferRequest;
        uniformBufferRequest.bindingName  = "ubo";
        uniformBufferRequest.shaderStage  = FV_SHADER_STAGE_VERTEX;
        uniformBufferRequest.shaderModule = shaderModule;
        uint32_t uniformBufferBindingPoint;

        if (fvShaderModuleGetBindingPoint(&uniformBufferBindingPoint,
                                          &uniformBufferRequest) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error(
                "Failed to find uniform buffer binding point in shader!");
        }

        std::array<FvWriteDescriptorSet, 1> descriptorWrites = {};
        descriptorWrites[0].dstSet          = descriptorSet;
        descriptorWrites[0].dstBinding      = uniformBufferBindingPoint;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType  = FV_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].bufferInfo      = &bufferInfo;

        fvUpdateDescriptorSets(descriptorWrites.size(),
                               descriptorWrites.data());
    }

    void updateUniformBuffer() {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::milliseconds>(
                         currentTime - startTime)
                         .count() /
                     1000.0f;

        UniformBufferObject ubo = {};
        ubo.model = glm::mat4();
        ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f),
                               glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj =
            glm::perspective(glm::radians(45.0f),
                             outputWidth / (float)outputHeight, 0.1f, 10.0f);

        fvBufferReplaceData(uniformBuffer, &ubo, sizeof(ubo));
    }

    void createFramebuffer() {
        std::array<FvImage, 2> attachments = {swapchainImage, depthImage};

        FvFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.renderPass              = renderPass;
        framebufferInfo.attachmentCount         = attachments.size();
        framebufferInfo.attachments             = attachments.data();
        framebufferInfo.width                   = outputWidth;
        framebufferInfo.height                  = outputHeight;
        framebufferInfo.layers                  = 1;

        if (fvFramebufferCreate(framebuffer.replace(), &framebufferInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }

    void createCommandPool() {
        FvCommandPoolCreateInfo poolInfo = {};

        if (fvCommandPoolCreate(commandPool.replace(), &poolInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }
    }

    void createCommandBuffer() {
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

        std::array<FvClearValue, 2> clearValues = {};
        clearValues[0].color        = {0.2f, 0.2f, 0.2f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = clearValues.size();
        renderPassInfo.clearValues     = clearValues.data();

        fvCmdBeginRenderPass(commandBuffer, &renderPassInfo);

        {
            fvCmdBindGraphicsPipeline(commandBuffer, graphicsPipeline);

            FvBuffer vertexBuffers[] = {vertexBuffer};
            FvSize offsets[]         = {0};
            fvCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            fvCmdBindIndexBuffer(commandBuffer, indexBuffer, 0,
                                 FV_INDEX_TYPE_UINT32);

            fvCmdBindDescriptorSets(commandBuffer, pipelineLayout, 0, 1,
                                    &descriptorSet);

            // fvCmdDraw(commandBuffer, vertices.size(), 1, 0, 0);
            fvCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
        }

        fvCmdEndRenderPass(commandBuffer);

        if (fvCommandBufferEnd(commandBuffer) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer");
        }
    }

    void createDescriptorSet() {
        FvDescriptorInfo uboLayoutBinding = {};
        uboLayoutBinding.binding          = 1;
        uboLayoutBinding.descriptorType   = FV_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount  = 1;
        uboLayoutBinding.stageFlags       = FV_SHADER_STAGE_VERTEX;

        std::array<FvDescriptorInfo, 1> descriptors = {uboLayoutBinding};

        FvDescriptorSetCreateInfo descriptorSetInfo = {};
        descriptorSetInfo.descriptorCount           = descriptors.size();
        descriptorSetInfo.descriptors               = descriptors.data();

        if (fvDescriptorSetCreate(descriptorSet.replace(),
                                  &descriptorSetInfo) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set");
        }
    }

    void createGraphicsPipeline() {
        std::vector<char> shaderCode =
            readFile("src/projects/triangle/assets/triangle.metal");
        shaderCode.push_back('\0');

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

        auto bindingDescription    = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexBindingDescriptions     = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount =
            attributeDescriptions.size();
        vertexInputInfo.vertexAttributeDescriptions =
            attributeDescriptions.data();

        FvPipelineInputAssemblyDescription inputAssembly = {};
        inputAssembly.primitiveType          = FV_PRIMITIVE_TYPE_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = FV_TRUE;

        FvViewport viewport = {};
        viewport.x          = 0.0f;
        viewport.y          = 0.0f;
        viewport.width      = (float)outputWidth;
        viewport.height     = (float)outputHeight;
        viewport.minDepth   = 0.0f;
        viewport.maxDepth   = 1.0f;

        FvRect2D scissor      = {};
        scissor.origin        = {0, 0};
        scissor.extent.width  = (uint32_t)outputWidth;
        scissor.extent.height = (uint32_t)outputHeight;

        FvPipelineViewportDescription viewportState = {};
        viewportState.viewport                      = viewport;
        viewportState.scissor                       = scissor;

        FvPipelineRasterizerDescription rasterizer = {};
        rasterizer.depthClampEnable                = FV_FALSE;
        /* rasterizer.rasterizerDiscardEnable         = FV_FALSE; */
        rasterizer.cullMode    = FV_CULL_MODE_BACK;
        rasterizer.frontFacing = FV_WINDING_ORDER_COUNTER_CLOCKWISE;

        FvPipelineDepthStencilStateDescription depthStencil = {};
        depthStencil.depthWriteEnable                       = FV_TRUE;
        depthStencil.depthCompareFunc  = FV_COMPARE_FUNC_LESS;
        depthStencil.stencilTestEnable = FV_FALSE;

        FvColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.blendEnable                 = FV_FALSE;
        colorBlendAttachment.colorWriteMask = (FvColorComponentFlags)(
            FV_COLOR_COMPONENT_R | FV_COLOR_COMPONENT_G | FV_COLOR_COMPONENT_B |
            FV_COLOR_COMPONENT_A);

        FvPipelineColorBlendStateDescription colorBlending = {};
        colorBlending.attachmentCount                      = 1;
        colorBlending.attachments = &colorBlendAttachment;

        FvDescriptorSet descriptorSets[]              = {descriptorSet};
        FvPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.descriptorSetCount         = 1;
        pipelineLayoutInfo.descriptorSets             = descriptorSets;
        pipelineLayoutInfo.pushConstantRangeCount     = 0;
        pipelineLayoutInfo.pushConstantRanges         = nullptr;

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
        pipelineInfo.depthStencilDescription      = &depthStencil;
        pipelineInfo.layout                       = pipelineLayout;
        pipelineInfo.renderPass                   = renderPass;
        pipelineInfo.subpass                      = 0;

        if (fvGraphicsPipelineCreate(graphicsPipeline.replace(),
                                     &pipelineInfo) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
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

    void createDepthResources() {
        FvImageCreateInfo imageInfo = {};
        imageInfo.imageType         = FV_IMAGE_TYPE_2D;
        imageInfo.extent.width      = outputWidth;
        imageInfo.extent.height     = outputHeight;
        imageInfo.extent.depth      = 1;
        imageInfo.mipLevels         = 1;
        imageInfo.arrayLayers       = 1;
        imageInfo.format            = FV_FORMAT_DEPTH32FLOAT;
        imageInfo.usage             = FV_IMAGE_USAGE_RENDER_TARGET;
        imageInfo.samples           = FV_SAMPLE_COUNT_1;

        if (fvImageCreate(depthImage.replace(), &imageInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create depth image!");
        }
    }

    void drawFrame() {
        if (fvAcquireNextImage(swapchain, imageAvailableSemaphore) !=
            FV_RESULT_SUCCESS) {
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
                        int outputWidth  = 0;
                        int outputHeight = 0;

                        SDL_GL_GetDrawableSize(window, &outputWidth,
                                               &outputHeight);

                        recreateSwapchain();
                        break;
                    }
                    break;
                }
                }
            }

            updateUniformBuffer();
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
    int outputWidth, outputHeight;

    FDeleter<FvPipelineLayout> pipelineLayout{fvPipelineLayoutDestroy};
    FDeleter<FvRenderPass> renderPass{fvRenderPassDestroy};
    FDeleter<FvGraphicsPipeline> graphicsPipeline{fvGraphicsPipelineDestroy};
    FDeleter<FvFramebuffer> framebuffer{fvFramebufferDestroy};
    FDeleter<FvCommandPool> commandPool{fvCommandPoolDestroy};
    // Resources of command buffer automatically freed
    FvCommandBuffer commandBuffer;
    FDeleter<FvSwapchain> swapchain{fvDestroySwapchain};
    FvImage swapchainImage; // TODO: wrap in FDeleter?
    FDeleter<FvSemaphore> imageAvailableSemaphore{fvSemaphoreDestroy};
    FDeleter<FvSemaphore> renderFinishedSemaphore{fvSemaphoreDestroy};

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    FDeleter<FvBuffer> vertexBuffer{fvBufferDestroy};
    FDeleter<FvBuffer> indexBuffer{fvBufferDestroy};

    FDeleter<FvBuffer> uniformBuffer{fvBufferDestroy};
    FDeleter<FvDescriptorSet> descriptorSet{fvDescriptorSetDestroy};
    FDeleter<FvImage> depthImage{fvImageDestroy};

    FDeleter<FvShaderModule> shaderModule{fvShaderModuleDestroy};
};

int main(void) {
    // Initialize SDL2
    SDL_Init(SDL_INIT_VIDEO);

    uint32_t flags = 0x0;
    flags |= SDL_WINDOW_RESIZABLE;

    // Create window
    SDL_Window *window =
        SDL_CreateWindow("Triangle", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 800, 600, flags);

    try {
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

#if FV_PLATFORM_MACOS
        FvMacOSSurfaceCreateInfo surfaceCreateInfo;
        surfaceCreateInfo.nsWindow = (void *)windowInfo.info.cocoa.window;

        if (fvCreateMacOSSurface(surface.replace(), &surfaceCreateInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to get MacOS surface.");
        }
#endif

        // Initialize Fever
        FvInitInfo initInfo;
        initInfo.surface = surface;

        if (fvInit(&initInfo) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to initialize Fever library.");
        }

        HelloTriangleApplication app(window);

        app.run();

    } catch (const std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }

    fvShutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
