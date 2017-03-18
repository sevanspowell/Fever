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

void fvDestroySurface(FvSurface surface) { surface = nullptr; }
