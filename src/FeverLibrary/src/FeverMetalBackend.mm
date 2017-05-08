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
    if (metalWrapper != nullptr) {
        metalWrapper->shutdown();

        delete metalWrapper;
        metalWrapper = nullptr;
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
    if (metalWrapper != nullptr) {
        metalWrapper->semaphoreDestroy(semaphore);
    }
}

/* FvResult fvGetDrawable(FvDrawable *drawable) { */
/*     if (metalWrapper != nullptr) { */
/*         return metalWrapper->getDrawable(drawable); */
/*     } else { */
/*         return FV_RESULT_FAILURE; */
/*     } */
/* } */

/* FvResult fvGetDrawableImage(FvImage *drawableImage, FvDrawable drawable) { */
/*     if (metalWrapper != nullptr) { */
/*         return metalWrapper->getDrawableImage(drawableImage, drawable); */
/*     } else { */
/*         return FV_RESULT_FAILURE; */
/*     } */
/* } */

FvResult fvCreateSwapchain(FvSwapchain *swapchain,
                           const FvSwapchainCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->createSwapchain(swapchain, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvDestroySwapchain(FvSwapchain swapchain) {
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
                            FvSemaphore imageAvailableSemaphore,
                            uint32_t *imageIndex) {
    if (metalWrapper != nullptr) {
        return metalWrapper->acquireNextImage(
            swapchain, imageAvailableSemaphore, imageIndex);
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
    if (metalWrapper != nullptr) {
        metalWrapper->framebufferDestroy(framebuffer);
    }
}

FvResult fvImageViewCreate(FvImageView *imageView,
                           const FvImageViewCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->imageViewCreate(imageView, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvImageViewDestroy(FvImageView imageView) {
    if (metalWrapper != nullptr) {
        metalWrapper->imageViewDestroy(imageView);
    }
}

FvResult fvImageCreate(FvImage *image, const FvImageCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->imageCreate(image, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvImageDestroy(FvImage image) {
    if (metalWrapper != nullptr) {
        metalWrapper->imageDestroy(image);
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

void fvPipelineLayoutDestroy(FvPipelineLayout layout) {}

FvResult fvShaderModuleCreate(FvShaderModule *shaderModule,
                              const FvShaderModuleCreateInfo *createInfo) {
    if (metalWrapper != nullptr) {
        return metalWrapper->shaderModuleCreate(shaderModule, createInfo);
    } else {
        return FV_RESULT_FAILURE;
    }
}

void fvShaderModuleDestroy(FvShaderModule shaderModule) {
    if (metalWrapper != nullptr) {
        metalWrapper->shaderModuleDestroy(shaderModule);
    }
}

FvResult fvCreateCocoaSurface(FvSurface *surface,
                              const FvCocoaSurfaceCreateInfo *createInfo) {

    FvResult result = FV_RESULT_FAILURE;

    CAMetalLayer *metalLayer = NULL;

    if (surface != NULL && createInfo != NULL) {
        // Attempt to get a CAMetalLayer from MTKView
        if (NSClassFromString(@"MTKView") != NULL) {
            MTKView *view = (MTKView *)createInfo->view;

            if (view != NULL &&
                [view isKindOfClass:NSClassFromString(@"MTKView")]) {
                metalLayer = (CAMetalLayer *)view.layer;
            }
        }

        // Attempt to get CAMetalLayer from NSWindow if previous attempt failed.
        if (NSClassFromString(@"CAMetalLayer") != NULL) {
            if (metalLayer == NULL) {
                NSView *view = (NSView *)createInfo->view;
                [view setWantsLayer:YES];
                metalLayer = [CAMetalLayer layer];
                [view setLayer:metalLayer];
            }
        }

        if (metalLayer != NULL) {
            *surface = (FvSurface)metalLayer;
            result   = FV_RESULT_SUCCESS;
        }
    }

    return result;
}

void fvDestroySurface(FvSurface surface) {
    CAMetalLayer *metalLayer = (CAMetalLayer *)surface;

    [metalLayer release];
    metalLayer = nil;
}

void fvDeviceWaitIdle() {
    
}
