/*===-- Fever/FeverMetalBackend.h - Metal backend -------------------*- C -*-===
 *
 *                     The Fever Graphics Library
 *
 * This file is distributed under the MIT License. See LICENSE.txt for details.
 *===----------------------------------------------------------------------===*/
/**
 * \file
 * \brief Apple Metal implementation for Fever Graphics Library.
 *
 *===----------------------------------------------------------------------===*/
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <Fever/Fever.h>
#include <Fever/FeverMetalWrapper.h>
#include <Fever/FeverSurfaceAcquisition.h>

static fv::MetalWrapper *metalWrapper = nullptr;

FvResult fvInit(const FvInitInfo *initInfo) {
    FvResult result = FV_RESULT_FAILURE;

    metalWrapper = new fv::MetalWrapper();

    if (metalWrapper != nullptr &&
        metalWrapper->init(initInfo) == FV_RESULT_SUCCESS) {
        result = FV_RESULT_SUCCESS;
    }

    return result;
}

void fvShutdown() {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->shutdown();

        delete metalWrapper;
        metalWrapper = nullptr;
    }
}

void fvUpdateDescriptorSets(uint32_t descriptorWriteCount,
                            const FvWriteDescriptorSet *descriptorWrites) {
    if (metalWrapper != nullptr) {
        return metalWrapper->updateDescriptorSets(descriptorWriteCount,
                                                  descriptorWrites);
    }
}

FvResult fvSemaphoreCreate(FvSemaphore *semaphore) {
    if (metalWrapper != nullptr) {
        return metalWrapper->semaphoreCreate(semaphore);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvSemaphoreDestroy(FvSemaphore semaphore) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->semaphoreDestroy(semaphore);
    }
}

FvResult fvCreateSwapchain(FvSwapchain *swapchain,
                           const FvSwapchainCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->createSwapchain(swapchain, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvDestroySwapchain(FvSwapchain swapchain) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->destroySwapchain(swapchain);
    }
}

void fvGetSwapchainImage(FvSwapchain swapchain, FvImage *swapchainImage) {
    if (metalWrapper != nullptr) {
        metalWrapper->getSwapchainImage(swapchain, swapchainImage);
    }
}

FvResult fvAcquireNextImage(FvSwapchain swapchain,
                            FvSemaphore imageAvailableSemaphore) {
    if (metalWrapper != nullptr) {
        return metalWrapper->acquireNextImage(swapchain,
                                              imageAvailableSemaphore);
    } else {
        return FV_RESULT_FAILURE;
    }
}

FvResult fvQueueSubmit(uint32_t submissionsCount,
                       const FvSubmitInfo *submissions) {
    if (metalWrapper != nullptr) {
        return metalWrapper->queueSubmit(submissionsCount, submissions);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvQueuePresent(const FvPresentInfo *presentInfo) {
    if (metalWrapper != nullptr) {
        metalWrapper->queuePresent(presentInfo);
    }
}

void fvCmdBindGraphicsPipeline(FvCommandBuffer commandBuffer,
                               FvGraphicsPipeline graphicsPipeline) {
    if (metalWrapper != nullptr) {
        metalWrapper->cmdBindGraphicsPipeline(commandBuffer, graphicsPipeline);
    }
}

void fvCmdBindVertexBuffers(FvCommandBuffer commandBuffer,
                            uint32_t firstBinding, uint32_t bindingCount,
                            const FvBuffer *buffers, const FvSize *offsets) {
    if (metalWrapper != nullptr) {
        metalWrapper->cmdBindVertexBuffers(commandBuffer, firstBinding,
                                           bindingCount, buffers, offsets);
    }
}

void fvCmdDraw(FvCommandBuffer commandBuffer, uint32_t vertexCount,
               uint32_t instanceCount, uint32_t firstVertex,
               uint32_t firstInstance) {
    if (metalWrapper != nullptr) {
        metalWrapper->cmdDraw(commandBuffer, vertexCount, instanceCount,
                              firstVertex, firstInstance);
    }
}

void fvCmdBeginRenderPass(FvCommandBuffer commandBuffer,
                          const FvRenderPassBeginInfo *renderPassInfo) {
    if (metalWrapper != nullptr) {
        metalWrapper->cmdBeginRenderPass(commandBuffer, renderPassInfo);
    }
}

void fvCmdEndRenderPass(FvCommandBuffer commandBuffer) {
    if (metalWrapper != nullptr) {
        metalWrapper->cmdEndRenderPass(commandBuffer);
    }
}

FvResult fvCommandBufferCreate(FvCommandBuffer *commandBuffer,
                               FvCommandPool commandPool) {
    if (metalWrapper != nullptr) {
        return metalWrapper->commandBufferCreate(commandBuffer, commandPool);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvCommandBufferDestroy(FvCommandBuffer commandBuffer,
                            FvCommandPool commandPool) {
    assert(metalWrapper != nullptr);
}

void fvCommandBufferBegin(FvCommandBuffer commandBuffer) {
    if (metalWrapper != nullptr) {
        metalWrapper->commandBufferBegin(commandBuffer);
    }
}

FvResult fvCommandBufferEnd(FvCommandBuffer commandBuffer) {
    if (metalWrapper != nullptr) {
        return metalWrapper->commandBufferEnd(commandBuffer);
    } else {
        return FV_RESULT_FAILURE;
    }
}

FvResult fvCommandPoolCreate(FvCommandPool *commandPool,
                             const FvCommandPoolCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->commandPoolCreate(commandPool, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvCommandPoolDestroy(FvCommandPool commandPool) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->commandPoolDestroy(commandPool);
    }
}

FvResult fvFramebufferCreate(FvFramebuffer *framebuffer,
                             const FvFramebufferCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->framebufferCreate(framebuffer, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvFramebufferDestroy(FvFramebuffer framebuffer) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->framebufferDestroy(framebuffer);
    }
}

FvResult fvImageCreate(FvImage *image, const FvImageCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->imageCreate(image, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvImageReplaceRegion(FvImage image, FvRect3D region, uint32_t mipLevel,
                          uint32_t layer, void *data, size_t bytesPerRow,
                          size_t bytesPerImage) {
    if (metalWrapper != nullptr) {
        return metalWrapper->imageReplaceRegion(
            image, region, mipLevel, layer, data, bytesPerRow, bytesPerImage);
    }
}

void fvImageDestroy(FvImage image) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->imageDestroy(image);
    }
}

FvResult fvSamplerCreate(FvSampler *sampler,
                         const FvSamplerCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->samplerCreate(sampler, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvSamplerDestroy(FvSampler sampler) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->samplerDestroy(sampler);
    }
}

FvResult
fvGraphicsPipelineCreate(FvGraphicsPipeline *graphicsPipeline,
                         const FvGraphicsPipelineCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->graphicsPipelineCreate(graphicsPipeline,
                                                    createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvGraphicsPipelineDestroy(FvGraphicsPipeline graphicsPipeline) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->graphicsPipelineDestroy(graphicsPipeline);
    }
}

FvResult fvRenderPassCreate(FvRenderPass *renderPass,
                            const FvRenderPassCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->renderPassCreate(renderPass, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvRenderPassDestroy(FvRenderPass renderPass) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->renderPassDestroy(renderPass);
    }
}

FvResult fvPipelineLayoutCreate(FvPipelineLayout *layout,
                                const FvPipelineLayoutCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->pipelineLayoutCreate(layout, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvPipelineLayoutDestroy(FvPipelineLayout layout) {
    assert(metalWrapper != nullptr);

    if (metalWrapper != nullptr) {
        metalWrapper->pipelineLayoutDestroy(layout);
    }
}

FvResult fvShaderModuleCreate(FvShaderModule *shaderModule,
                              const FvShaderModuleCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->shaderModuleCreate(shaderModule, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvShaderModuleDestroy(FvShaderModule shaderModule) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->shaderModuleDestroy(shaderModule);
    }
}

FvResult
fvShaderModuleGetBindingPoint(uint32_t *bindingPoint,
                              const FvShaderReflectionRequest *request) {
    if (metalWrapper != nullptr) {
        return metalWrapper->shaderModuleGetBindingPoint(bindingPoint, request);
    } else {
        return FV_RESULT_FAILURE;
    }
}

FvResult fvCreateMacOSSurface(FvSurface *surface,
                              const FvMacOSSurfaceCreateInfo *createInfo) {
    CAMetalLayer *metalLayer = NULL;

    if (surface == NULL || createInfo == NULL) {
        return FV_RESULT_FAILURE;
    }

    // Attempt to get a CAMetalLayer from MTKView
    if (NSClassFromString(@"MTKView") != NULL) {
        MTKView *view =
            (MTKView *)((NSWindow *)createInfo->nsWindow).contentView;

        if (view != NULL &&
            [view isKindOfClass:NSClassFromString(@"MTKView")]) {
            metalLayer = (CAMetalLayer *)view.layer;
        }
    }

    // Attempt to get CAMetalLayer from NSWindow if previous attempt failed.
    if (metalLayer == NULL) {
        if (NSClassFromString(@"CAMetalLayer") != NULL) {
            NSView *view =
                (NSView *)((NSWindow *)createInfo->nsWindow).contentView;
            [view setWantsLayer:YES];
            metalLayer = [CAMetalLayer layer];
            [view setLayer:metalLayer];
        }
    }

    if (metalLayer == NULL) {
        return FV_RESULT_FAILURE;
    }

    *surface = (FvSurface)metalLayer;

    return FV_RESULT_SUCCESS;
}

void fvDestroySurface(FvSurface surface) {
    assert(metalWrapper != nullptr);

    CAMetalLayer *metalLayer = (CAMetalLayer *)surface;

    [metalLayer release];
    metalLayer = nil;
}

void fvDeviceWaitIdle() {}

FvResult fvBufferCreate(FvBuffer *buffer,
                        const FvBufferCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->bufferCreate(buffer, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvBufferDestroy(FvBuffer buffer) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        return metalWrapper->bufferDestroy(buffer);
    }
}

void fvBufferReplaceData(FvBuffer buffer, void *data, size_t dataSize) {
    if (metalWrapper != nullptr) {
        return metalWrapper->bufferReplaceData(buffer, data, dataSize);
    }
}

void fvCmdBindIndexBuffer(FvCommandBuffer commandBuffer, FvBuffer buffer,
                          FvSize offset, FvIndexType indexType) {
    if (metalWrapper != nullptr) {
        return metalWrapper->cmdBindIndexBuffer(commandBuffer, buffer, offset,
                                                indexType);
    }
}

void fvCmdBindDescriptorSets(FvCommandBuffer commandBuffer,
                             FvPipelineLayout layout, uint32_t firstSet,
                             uint32_t descriptorSetCount,
                             const FvDescriptorSet *descriptorSets) {
    if (metalWrapper != nullptr) {
        metalWrapper->cmdBindDescriptorSets(commandBuffer, layout, firstSet,
                                            descriptorSetCount, descriptorSets);
    }
}

void fvCmdDrawIndexed(FvCommandBuffer commandBuffer, uint32_t indexCount,
                      uint32_t instanceCount, uint32_t firstIndex,
                      int32_t vertexOffset, uint32_t firstInstance) {
    if (metalWrapper != nullptr) {
        return metalWrapper->cmdDrawIndexed(commandBuffer, indexCount,
                                            instanceCount, firstIndex,
                                            vertexOffset, firstInstance);
    }
}

FvResult fvDescriptorSetCreate(FvDescriptorSet *descriptorSet,
                               const FvDescriptorSetCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->descriptorSetCreate(descriptorSet, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvDescriptorSetDestroy(FvDescriptorSet descriptorSet) {
    assert(metalWrapper != nullptr);
    if (metalWrapper != nullptr) {
        metalWrapper->descriptorSetDestroy(descriptorSet);
    }
}
