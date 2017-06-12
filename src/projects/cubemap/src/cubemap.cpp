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
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <Fever/Fever.h>
#include <Fever/FeverPlatform.h>
#include <Fever/FeverSurfaceAcquisition.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;

    static FvVertexInputBindingDescription getBindingDescription() {
        FvVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding                         = 0;
        bindingDescription.stride                          = sizeof(Vertex);
        bindingDescription.inputRate = FV_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<FvVertexInputAttributeDescription, 3>
    getAttributeDescriptions() {
        std::array<FvVertexInputAttributeDescription, 3> attributeDescriptions =
            {};

        attributeDescriptions[0].binding  = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format   = FV_VERTEX_FORMAT_FLOAT3;
        attributeDescriptions[0].offset   = offsetof(Vertex, pos);

        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format   = FV_VERTEX_FORMAT_FLOAT3;
        attributeDescriptions[1].offset   = offsetof(Vertex, normal);

        attributeDescriptions[2].binding  = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format   = FV_VERTEX_FORMAT_FLOAT2;
        attributeDescriptions[2].offset   = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex &other) const {
        return pos == other.pos && normal == other.normal &&
               texCoord == other.texCoord;
    }
};

struct SkyboxVertex {
    static std::array<FvVertexInputAttributeDescription, 1>
    getAttributeDescriptions() {
        std::array<FvVertexInputAttributeDescription, 1> attributeDescriptions =
            {};

        attributeDescriptions[0].binding  = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format   = FV_VERTEX_FORMAT_FLOAT3;
        attributeDescriptions[0].offset   = offsetof(Vertex, pos);

        return attributeDescriptions;
    }
};

namespace std {
template <> struct hash<Vertex> {
    size_t operator()(Vertex const &vertex) const {
        return ((hash<glm::vec3>()(vertex.pos) ^
                 (hash<glm::vec3>()(vertex.normal) << 1)) >>
                1) ^
               (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};
}

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 invTransposeModel;
    glm::mat4 invModelView;
    glm::vec3 worldCameraPosition;
};

struct SkyboxUniformBufferObject {
    glm::mat4 proj;
    glm::mat4 view;
    glm::mat4 model;
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

        createCommandPool();
        createDepthResources();
        createFramebuffer();

        createSkyboxTextureImage();
        createSkyboxTextureSampler();

        createDescriptorSet();
        createGraphicsPipeline();
        loadModel(MODEL_PATH, modelVertices, modelIndices);
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffer();
        writeDescriptorSet();

        createSkyboxDescriptorSet();
        createSkyboxGraphicsPipeline();
        loadModel(SKYBOX_MODEL_PATH, skyboxVertices, skyboxIndices);
        createSkyboxVertexBuffer();
        createSkyboxIndexBuffer();
        createSkyboxUniformBuffer();
        writeSkyboxDescriptorSet();

        createCommandBuffer();
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

    void recreateSwapchain() {
        fvDeviceWaitIdle();

        createSwapchain();
        createDepthResources();
        createRenderPass();
        createGraphicsPipeline();
        createSkyboxGraphicsPipeline();
        createFramebuffer();
        createCommandBuffer();
    }

    void loadModel(const std::string &modelPath, std::vector<Vertex> &vertices,
                   std::vector<uint32_t> &indices) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
                              modelPath.c_str())) {
            throw std::runtime_error(err);
        }

        // std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

        for (const auto &shape : shapes) {
            for (const auto &index : shape.mesh.indices) {
                Vertex vertex = {};

                vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                              attrib.vertices[3 * index.vertex_index + 1],
                              attrib.vertices[3 * index.vertex_index + 2]};

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

                vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                                 attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};

                vertices.push_back(vertex);
                indices.push_back(indices.size());

                // if (uniqueVertices.count(vertex) == 0) {
                //     uniqueVertices[vertex] =
                //         static_cast<uint32_t>(vertices.size());
                //     vertices.push_back(vertex);
                // }

                // indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

    void createVertexBuffer() {
        FvBufferCreateInfo bufferInfo = {};
        bufferInfo.size  = sizeof(modelVertices[0]) * modelVertices.size();
        bufferInfo.usage = FV_BUFFER_USAGE_VERTEX_BUFFER;
        bufferInfo.data  = modelVertices.data();

        if (fvBufferCreate(vertexBuffer.replace(), &bufferInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create vertex buffer!");
        }
    }

    void createSkyboxVertexBuffer() {
        FvBufferCreateInfo bufferInfo = {};
        bufferInfo.size  = sizeof(skyboxVertices[0]) * skyboxVertices.size();
        bufferInfo.usage = FV_BUFFER_USAGE_VERTEX_BUFFER;
        bufferInfo.data  = skyboxVertices.data();

        if (fvBufferCreate(skyboxVertexBuffer.replace(), &bufferInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create vertex buffer!");
        }
    }

    void createIndexBuffer() {
        FvBufferCreateInfo bufferInfo = {};
        bufferInfo.size  = sizeof(modelIndices[0]) * modelIndices.size();
        bufferInfo.usage = FV_BUFFER_USAGE_INDEX_BUFFER;
        bufferInfo.data  = modelIndices.data();

        if (fvBufferCreate(indexBuffer.replace(), &bufferInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create vertex buffer!");
        }
    }

    void createSkyboxIndexBuffer() {
        FvBufferCreateInfo bufferInfo = {};
        bufferInfo.size  = sizeof(skyboxIndices[0]) * skyboxIndices.size();
        bufferInfo.usage = FV_BUFFER_USAGE_INDEX_BUFFER;
        bufferInfo.data  = skyboxIndices.data();

        if (fvBufferCreate(skyboxIndexBuffer.replace(), &bufferInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create vertex buffer!");
        }
    }

    void createUniformBuffer() {
        FvBufferCreateInfo bufferInfo = {};
        bufferInfo.size               = sizeof(UniformBufferObject);
        bufferInfo.usage              = FV_BUFFER_USAGE_UNIFORM_BUFFER;
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

        FvDescriptorImageInfo imageInfo = {};
        imageInfo.image                 = skyboxTextureImage;
        imageInfo.sampler               = skyboxTextureSampler;

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

        uniformBufferRequest.bindingName  = "ubo";
        uniformBufferRequest.shaderStage  = FV_SHADER_STAGE_FRAGMENT;
        uniformBufferRequest.shaderModule = shaderModule;
        uint32_t uniformBufferBindingPointFragment;

        if (fvShaderModuleGetBindingPoint(&uniformBufferBindingPointFragment,
                                          &uniformBufferRequest) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error(
                "Failed to find uniform buffer binding point in shader!");
        }

        FvShaderReflectionRequest diffuseTextureRequest;
        diffuseTextureRequest.bindingName  = "cubemapTexture";
        diffuseTextureRequest.shaderStage  = FV_SHADER_STAGE_FRAGMENT;
        diffuseTextureRequest.shaderModule = shaderModule;
        uint32_t diffuseTextureBindingPoint;

        if (fvShaderModuleGetBindingPoint(&diffuseTextureBindingPoint,
                                          &diffuseTextureRequest) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error(
                "Failed to find diffuse texture binding point in shader!");
        }

        std::array<FvWriteDescriptorSet, 3> descriptorWrites = {};
        descriptorWrites[0].dstSet          = descriptorSet;
        descriptorWrites[0].dstBinding      = uniformBufferBindingPoint;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType  = FV_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].bufferInfo      = &bufferInfo;

        descriptorWrites[1].dstSet          = descriptorSet;
        descriptorWrites[1].dstBinding      = diffuseTextureBindingPoint;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType =
            FV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].imageInfo       = &imageInfo;

        descriptorWrites[2].dstSet          = descriptorSet;
        descriptorWrites[2].dstBinding      = uniformBufferBindingPointFragment;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType  = FV_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].bufferInfo      = &bufferInfo;

        fvUpdateDescriptorSets(descriptorWrites.size(),
                               descriptorWrites.data());
    }

    void createSkyboxUniformBuffer() {
        FvBufferCreateInfo bufferInfo = {};
        bufferInfo.size               = sizeof(SkyboxUniformBufferObject);
        bufferInfo.usage              = FV_BUFFER_USAGE_UNIFORM_BUFFER;
        bufferInfo.data = nullptr; // Add data in updateUniformBuffer

        if (fvBufferCreate(skyboxUniformBuffer.replace(), &bufferInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create vertex buffer!");
        }
    }

    void writeSkyboxDescriptorSet() {
        FvDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer                 = skyboxUniformBuffer;
        bufferInfo.offset                 = 0;
        bufferInfo.range                  = sizeof(SkyboxUniformBufferObject);

        FvDescriptorImageInfo imageInfo = {};
        imageInfo.image                 = skyboxTextureImage;
        imageInfo.sampler               = skyboxTextureSampler;

        // Get shader reflection information
        FvShaderReflectionRequest uniformBufferRequest;
        uniformBufferRequest.bindingName  = "ubo";
        uniformBufferRequest.shaderStage  = FV_SHADER_STAGE_VERTEX;
        uniformBufferRequest.shaderModule = skyboxShaderModule;
        uint32_t uniformBufferBindingPoint;

        if (fvShaderModuleGetBindingPoint(&uniformBufferBindingPoint,
                                          &uniformBufferRequest) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error(
                "Failed to find uniform buffer binding point in shader!");
        }

        FvShaderReflectionRequest cubemapTextureRequest;
        cubemapTextureRequest.bindingName  = "cubemapTexture";
        cubemapTextureRequest.shaderStage  = FV_SHADER_STAGE_FRAGMENT;
        cubemapTextureRequest.shaderModule = skyboxShaderModule;
        uint32_t cubemapTextureBindingPoint;

        if (fvShaderModuleGetBindingPoint(&cubemapTextureBindingPoint,
                                          &cubemapTextureRequest) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error(
                "Failed to find diffuse texture binding point in shader!");
        }

        std::array<FvWriteDescriptorSet, 2> descriptorWrites = {};
        descriptorWrites[0].dstSet          = skyboxDescriptorSet;
        descriptorWrites[0].dstBinding      = uniformBufferBindingPoint;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType  = FV_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].bufferInfo      = &bufferInfo;

        descriptorWrites[1].dstSet          = skyboxDescriptorSet;
        descriptorWrites[1].dstBinding      = cubemapTextureBindingPoint;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType =
            FV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].imageInfo       = &imageInfo;

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
        // ubo.model = glm::rotate(glm::mat4(), time * glm::radians(90.0f),
        //                         glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.model = glm::scale(glm::mat4(), glm::vec3(0.15f, 0.15f, 0.15f));
        ubo.view  = glm::lookAt(
            glm::vec3(10.0f * cos(0.2f * time), 0.0f, 10.0f * sin(0.2f * time)),
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // ubo.view = glm::lookAt(glm::vec3(6.0f, 5.0f, 10.0f),
        //                        glm::vec3(6.0f, 5.0f, 0.0f),
        //                        glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj =
            glm::perspective(glm::radians(60.0f),
                             outputWidth / (float)outputHeight, 0.01f, 256.0f);
        ubo.invTransposeModel = glm::inverse(glm::transpose(ubo.model));
        ubo.invModelView      = glm::inverse(ubo.view * ubo.model);
        ubo.worldCameraPosition =
            glm::vec3(-ubo.view[3].x, -ubo.view[3].y, -ubo.view[3].z);

        fvBufferReplaceData(uniformBuffer, &ubo, sizeof(ubo));

        SkyboxUniformBufferObject skyboxUbo = {};
        skyboxUbo.proj                      = ubo.proj;
        skyboxUbo.view                      = ubo.view;
        skyboxUbo.model = glm::scale(glm::mat4(), glm::vec3(5.0f, 5.0f, 5.0f));

        fvBufferReplaceData(skyboxUniformBuffer, &skyboxUbo, sizeof(skyboxUbo));
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

    void createTextureImage() {
        int texWidth, texHeight, texChannels;
        stbi_uc *pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight,
                                    &texChannels, STBI_rgb_alpha);
        FvSize imageSize = texWidth * texHeight * 4; // We forced image to load
                                                     // with 4 channels using
                                                     // STBI_rgb_alpha flag

        if (!pixels) {
            throw std::runtime_error("Failed to load texture image!");
        }

        // Create image
        FvImageCreateInfo imageInfo = {};
        imageInfo.imageType         = FV_IMAGE_TYPE_2D;
        imageInfo.extent.width      = texWidth;
        imageInfo.extent.height     = texHeight;
        imageInfo.extent.depth      = 1;
        imageInfo.mipLevels         = 1;
        imageInfo.arrayLayers       = 1;
        imageInfo.format            = FV_FORMAT_RGBA8UNORM;
        imageInfo.usage             = FV_IMAGE_USAGE_SHADER_READ;
        imageInfo.samples           = FV_SAMPLE_COUNT_1;

        if (fvImageCreate(textureImage.replace(), &imageInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create image!");
        }

        // Upload image data to image object
        FvRect3D region;
        region.origin = {0, 0, 0};
        region.extent = {(uint32_t)texWidth, (uint32_t)texHeight, 1};
        fvImageReplaceRegion(textureImage, region, 0, 0, pixels, texWidth * 4,
                             0);

        stbi_image_free(pixels);
    }

    void createTextureSampler() {
        FvSamplerCreateInfo samplerInfo   = {};
        samplerInfo.magFilter             = FV_MIN_MAG_FILTER_LINEAR;
        samplerInfo.minFilter             = FV_MIN_MAG_FILTER_LINEAR;
        samplerInfo.addressModeU          = FV_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV          = FV_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW          = FV_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable      = FV_TRUE;
        samplerInfo.maxAnisotropy         = 16;
        samplerInfo.borderColor           = FV_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.compareEnable         = FV_FALSE;
        samplerInfo.compareFunc           = FV_COMPARE_FUNC_ALWAYS;
        samplerInfo.mipmapMode            = FV_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.normalizedCoordinates = FV_TRUE;

        if (fvSamplerCreate(textureSampler.replace(), &samplerInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create texture sampler!");
        }
    }

    void createSkyboxTextureImage() {
        bool imageCreated = false;

        // 0 - Positive X
        // 1 - Negative X
        // 2 - Positive Y
        // 3 - Negative Y
        // 4 - Positive Z
        // 5 - Negative Z
        for (unsigned int layer = 0; layer < 6; ++layer) {
            // Fetch image data for cube face
            std::stringstream skyboxFacePath;
            skyboxFacePath << SKYBOX_TEXTURE_BASE_PATH << layer << ".png";

            int faceWidth, faceHeight, faceChannels;
            stbi_uc *pixels =
                stbi_load(skyboxFacePath.str().c_str(), &faceWidth, &faceHeight,
                          &faceChannels, STBI_rgb_alpha);

            if (!pixels) {
                std::stringstream err;
                err << "Failed to load skybox face texture: "
                    << skyboxFacePath.str();

                throw std::runtime_error(err.str());
            }

            // Create image only once
            if (imageCreated == false) {

                // Create image
                FvImageCreateInfo imageInfo = {};
                imageInfo.imageType         = FV_IMAGE_TYPE_3D;
                imageInfo.extent.width      = faceWidth;
                imageInfo.extent.height     = faceHeight;
                imageInfo.extent.depth      = 1;
                imageInfo.mipLevels         = 1;
                imageInfo.arrayLayers       = 1;
                imageInfo.format            = FV_FORMAT_RGBA8UNORM;
                imageInfo.usage             = FV_IMAGE_USAGE_SHADER_READ;
                imageInfo.samples           = FV_SAMPLE_COUNT_1;

                if (fvImageCreate(skyboxTextureImage.replace(), &imageInfo) !=
                    FV_RESULT_SUCCESS) {
                    throw std::runtime_error("Failed to create image!");
                }

                imageCreated = true;
            }

            FvSize bytesPerRow = faceWidth * 4; // We forced image to load with
                                                // 4 channels using
                                                // STBI_rgb_alpha flag
            FvSize bytesPerImage = faceWidth * faceHeight *
                                   4; // We forced image to load with 4 channels
                                      // using STBI_rgb_alpha flag

            // Upload image data to image object
            FvRect3D region;
            region.origin = {0, 0, 0};
            region.extent = {(uint32_t)faceWidth, (uint32_t)faceHeight, 1};

            fvImageReplaceRegion(skyboxTextureImage, region, 0, layer, pixels,
                                 bytesPerRow, bytesPerImage);

            stbi_image_free(pixels);
        }
    }

    void createSkyboxTextureSampler() {
        FvSamplerCreateInfo samplerInfo   = {};
        samplerInfo.magFilter             = FV_MIN_MAG_FILTER_LINEAR;
        samplerInfo.minFilter             = FV_MIN_MAG_FILTER_NEAREST;
        samplerInfo.addressModeU          = FV_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV          = FV_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW          = FV_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable      = FV_TRUE;
        samplerInfo.maxAnisotropy         = 16;
        samplerInfo.borderColor           = FV_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.compareEnable         = FV_FALSE;
        samplerInfo.compareFunc           = FV_COMPARE_FUNC_ALWAYS;
        samplerInfo.mipmapMode            = FV_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.normalizedCoordinates = FV_TRUE;

        if (fvSamplerCreate(skyboxTextureSampler.replace(), &samplerInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create texture sampler!");
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
            // Skybox //
            fvCmdBindGraphicsPipeline(commandBuffer, skyboxGraphicsPipeline);

            FvBuffer skyboxVertexBuffers[] = {skyboxVertexBuffer};
            FvSize skyboxOffsets[]         = {0};
            fvCmdBindVertexBuffers(commandBuffer, 0, 1, skyboxVertexBuffers,
                                   skyboxOffsets);

            fvCmdBindIndexBuffer(commandBuffer, skyboxIndexBuffer, 0,
                                 FV_INDEX_TYPE_UINT32);

            fvCmdBindDescriptorSets(commandBuffer, skyboxPipelineLayout, 0, 1,
                                    &skyboxDescriptorSet);

            fvCmdDrawIndexed(commandBuffer, skyboxIndices.size(), 1, 0, 0, 0);

            // Model //
            fvCmdBindGraphicsPipeline(commandBuffer, graphicsPipeline);

            FvBuffer vertexBuffers[] = {vertexBuffer};
            FvSize offsets[]         = {0};
            fvCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            fvCmdBindIndexBuffer(commandBuffer, indexBuffer, 0,
                                 FV_INDEX_TYPE_UINT32);

            fvCmdBindDescriptorSets(commandBuffer, pipelineLayout, 0, 1,
                                    &descriptorSet);

            fvCmdDrawIndexed(commandBuffer, modelIndices.size(), 1, 0, 0, 0);
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

        FvDescriptorInfo uboLayoutBindingFragment = {};
        uboLayoutBindingFragment.binding          = 2;
        uboLayoutBindingFragment.descriptorType =
            FV_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBindingFragment.descriptorCount = 1;
        uboLayoutBindingFragment.stageFlags      = FV_SHADER_STAGE_FRAGMENT;

        FvDescriptorInfo samplerLayoutBinding = {};
        samplerLayoutBinding.binding          = 0;
        samplerLayoutBinding.descriptorCount  = 1;
        samplerLayoutBinding.descriptorType =
            FV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.stageFlags = FV_SHADER_STAGE_FRAGMENT;

        std::array<FvDescriptorInfo, 3> descriptors = {
            uboLayoutBinding, uboLayoutBindingFragment, samplerLayoutBinding};

        FvDescriptorSetCreateInfo descriptorSetInfo = {};
        descriptorSetInfo.descriptorCount           = descriptors.size();
        descriptorSetInfo.descriptors               = descriptors.data();

        if (fvDescriptorSetCreate(descriptorSet.replace(),
                                  &descriptorSetInfo) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set");
        }
    }

    void createSkyboxDescriptorSet() {
        FvDescriptorInfo uboLayoutBinding = {};
        uboLayoutBinding.binding          = 1;
        uboLayoutBinding.descriptorType   = FV_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount  = 1;
        uboLayoutBinding.stageFlags       = FV_SHADER_STAGE_VERTEX;

        FvDescriptorInfo samplerLayoutBinding = {};
        samplerLayoutBinding.binding          = 0;
        samplerLayoutBinding.descriptorCount  = 1;
        samplerLayoutBinding.descriptorType =
            FV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.stageFlags = FV_SHADER_STAGE_FRAGMENT;

        std::array<FvDescriptorInfo, 2> descriptors = {uboLayoutBinding,
                                                       samplerLayoutBinding};

        FvDescriptorSetCreateInfo descriptorSetInfo = {};
        descriptorSetInfo.descriptorCount           = descriptors.size();
        descriptorSetInfo.descriptors               = descriptors.data();

        if (fvDescriptorSetCreate(skyboxDescriptorSet.replace(),
                                  &descriptorSetInfo) != FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set");
        }
    }

    void createGraphicsPipeline() {
        std::vector<char> shaderCode =
            readFile("src/projects/cubemap/assets/cubemap.metal");
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

    void createSkyboxGraphicsPipeline() {
        std::vector<char> shaderCode =
            readFile("src/projects/cubemap/assets/skybox.metal");
        shaderCode.push_back('\0');

        FvShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.data = (void *)shaderCode.data();
        shaderModuleCreateInfo.size = shaderCode.size();

        if (fvShaderModuleCreate(skyboxShaderModule.replace(),
                                 &shaderModuleCreateInfo) !=
            FV_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to create shader module!");
        }

        FvPipelineShaderStageDescription vertShaderStageInfo = {};
        vertShaderStageInfo.stage             = FV_SHADER_STAGE_VERTEX;
        vertShaderStageInfo.entryFunctionName = "vertFunc";
        vertShaderStageInfo.shaderModule      = skyboxShaderModule;

        FvPipelineShaderStageDescription fragShaderStageInfo = {};
        fragShaderStageInfo.stage             = FV_SHADER_STAGE_FRAGMENT;
        fragShaderStageInfo.entryFunctionName = "fragFunc";
        fragShaderStageInfo.shaderModule      = skyboxShaderModule;

        FvPipelineShaderStageDescription shaderStages[] = {vertShaderStageInfo,
                                                           fragShaderStageInfo};

        FvPipelineVertexInputDescription vertexInputInfo = {};

        auto bindingDescription    = Vertex::getBindingDescription();
        auto attributeDescriptions = SkyboxVertex::getAttributeDescriptions();

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
        rasterizer.cullMode    = FV_CULL_MODE_FRONT;
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

        FvDescriptorSet descriptorSets[]              = {skyboxDescriptorSet};
        FvPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.descriptorSetCount         = 1;
        pipelineLayoutInfo.descriptorSets             = descriptorSets;
        pipelineLayoutInfo.pushConstantRangeCount     = 0;
        pipelineLayoutInfo.pushConstantRanges         = nullptr;

        if (fvPipelineLayoutCreate(skyboxPipelineLayout.replace(),
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

        if (fvGraphicsPipelineCreate(skyboxGraphicsPipeline.replace(),
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

    const std::string TEXTURE_PATH =
        "src/projects/textureMapping/assets/metalplate01_rgba.jpg";
    const std::string SKYBOX_TEXTURE_BASE_PATH =
        "src/projects/cubemap/assets/cubemap_yokohama_";
    const std::string MODEL_PATH = "src/projects/cubemap/assets/sphere.obj";
    const std::string SKYBOX_MODEL_PATH =
        "src/projects/cubemap/assets/cube.obj";

    SDL_Window *window;
    int outputWidth, outputHeight;

    FDeleter<FvRenderPass> renderPass{fvRenderPassDestroy};
    FDeleter<FvFramebuffer> framebuffer{fvFramebufferDestroy};
    FDeleter<FvCommandPool> commandPool{fvCommandPoolDestroy};
    // Resources of command buffer automatically freed
    FvCommandBuffer commandBuffer;
    FDeleter<FvSwapchain> swapchain{fvDestroySwapchain};
    FvImage swapchainImage; // TODO: wrap in FDeleter?
    FDeleter<FvSemaphore> imageAvailableSemaphore{fvSemaphoreDestroy};
    FDeleter<FvSemaphore> renderFinishedSemaphore{fvSemaphoreDestroy};

    std::vector<Vertex> modelVertices;
    std::vector<uint32_t> modelIndices;
    FDeleter<FvBuffer> vertexBuffer{fvBufferDestroy};
    FDeleter<FvBuffer> indexBuffer{fvBufferDestroy};
    FDeleter<FvPipelineLayout> pipelineLayout{fvPipelineLayoutDestroy};
    FDeleter<FvGraphicsPipeline> graphicsPipeline{fvGraphicsPipelineDestroy};
    FDeleter<FvImage> textureImage{fvImageDestroy};
    FDeleter<FvSampler> textureSampler{fvSamplerDestroy};
    FDeleter<FvBuffer> uniformBuffer{fvBufferDestroy};
    FDeleter<FvShaderModule> shaderModule{fvShaderModuleDestroy};
    FDeleter<FvDescriptorSet> descriptorSet{fvDescriptorSetDestroy};

    std::vector<Vertex> skyboxVertices;
    std::vector<uint32_t> skyboxIndices;
    FDeleter<FvBuffer> skyboxVertexBuffer{fvBufferDestroy};
    FDeleter<FvBuffer> skyboxIndexBuffer{fvBufferDestroy};
    FDeleter<FvPipelineLayout> skyboxPipelineLayout{fvPipelineLayoutDestroy};
    FDeleter<FvGraphicsPipeline> skyboxGraphicsPipeline{
        fvGraphicsPipelineDestroy};
    FDeleter<FvImage> skyboxTextureImage{fvImageDestroy};
    FDeleter<FvSampler> skyboxTextureSampler{fvSamplerDestroy};
    FDeleter<FvBuffer> skyboxUniformBuffer{fvBufferDestroy};
    FDeleter<FvShaderModule> skyboxShaderModule{fvShaderModuleDestroy};
    FDeleter<FvDescriptorSet> skyboxDescriptorSet{fvDescriptorSetDestroy};

    FDeleter<FvImage> depthImage{fvImageDestroy};
};

int main(void) {
    // Initialize SDL2
    SDL_Init(SDL_INIT_VIDEO);

    uint32_t flags = 0x0;
    flags |= SDL_WINDOW_RESIZABLE;

    // Create window
    SDL_Window *window =
        SDL_CreateWindow("Cubemap", SDL_WINDOWPOS_CENTERED,
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
