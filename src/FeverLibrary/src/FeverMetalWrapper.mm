#include <Fever/FeverMetalWrapper.h>

namespace fv {
FvResult MetalWrapper::init(const FvInitInfo *initInfo) {
    FvResult result = FV_RESULT_FAILURE;

    if (initInfo == NULL || initInfo->surface == NULL) {
        return FV_RESULT_FAILURE;
    }

    // Get metal layer from surface
    metalLayer = (CAMetalLayer *)initInfo->surface;

    // Try to create device
    if (device == NULL) {
        device = MTLCreateSystemDefaultDevice();

        if (device == NULL) {
            return FV_RESULT_FAILURE;
        }
    }

    // Assign device to metal layer
    metalLayer.device = device;

    // Create a command queue
    commandQueue = [device newCommandQueue];

    // Check for nil command queue
    if (commandQueue == nil) {
        return FV_RESULT_FAILURE;
    }

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::shutdown() {
    FV_MTL_RELEASE(commandQueue);
    FV_MTL_RELEASE(device);
}

void MetalWrapper::cmdBindGraphicsPipeline(
    FvCommandBuffer commandBuffer, FvGraphicsPipeline graphicsPipeline) {
    // Get graphics pipeline wrapper
    GraphicsPipelineWrapper *pipelineWrapper = nullptr;

    const Handle *handle = (const Handle *)graphicsPipeline;

    if (handle != nullptr) {
        pipelineWrapper = graphicsPipelines.get(*handle);
    }

    // Get command buffer
    CommandBufferWrapper *commandBufferWrapper = nullptr;

    handle = (const Handle *)commandBuffer;

    if (handle != nullptr) {
        commandBufferWrapper = commandBuffers.get(*handle);
    }

    if (pipelineWrapper == nullptr || commandBufferWrapper == nullptr) {
        return;
    }

    if (pipelineWrapper->renderPass == nullptr) {
        return;
    }

    // Fill out render pass color and depthStencil attachment information
    for (uint32_t i = 0; i < pipelineWrapper->colorAttachments.size(); ++i) {
        uint32_t attachmentIndex =
            pipelineWrapper->colorAttachments[i].attachment;

        // Get clear color
        float clearColor[4] = {0, 0, 0, 0};
        for (uint32_t j = 0; j < 4; ++j) {
            clearColor[j] = commandBufferWrapper->clearValues[attachmentIndex]
                                .color.float32[j];
        }

        pipelineWrapper->renderPass.colorAttachments[i].texture =
            commandBufferWrapper->attachments[attachmentIndex];

        pipelineWrapper->renderPass.colorAttachments[i].clearColor =
            MTLClearColorMake(clearColor[0], clearColor[1], clearColor[2],
                              clearColor[3]);
    }

    // Should only be one depth stencil attachment per framebuffer
    if (pipelineWrapper->depthStencilAttachment.size() == 1) {
        uint32_t attachmentIndex =
            pipelineWrapper->depthStencilAttachment[0].attachment;

        pipelineWrapper->renderPass.depthAttachment.texture =
            commandBufferWrapper->attachments[attachmentIndex];
        pipelineWrapper->renderPass.stencilAttachment.texture =
            commandBufferWrapper->attachments[attachmentIndex];

        pipelineWrapper->renderPass.depthAttachment.clearDepth =
            commandBufferWrapper->clearValues[attachmentIndex]
                .depthStencil.depth;
        pipelineWrapper->renderPass.stencilAttachment.clearStencil =
            commandBufferWrapper->clearValues[attachmentIndex]
                .depthStencil.stencil;
    }

    // Associate graphics pipeline with command buffer
    commandBufferWrapper->graphicsPipeline = graphicsPipeline;
}

void MetalWrapper::cmdDraw(FvCommandBuffer commandBuffer, uint32_t vertexCount,
                           uint32_t instanceCount, uint32_t firstVertex,
                           uint32_t firstInstance) {

    // Get command buffer
    CommandBufferWrapper *commandBufferWrapper = nullptr;

    const Handle *handle = (const Handle *)commandBuffer;

    if (handle != nullptr) {
        commandBufferWrapper = commandBuffers.get(*handle);
    }

    if (commandBufferWrapper != nullptr) {
        commandBufferWrapper->drawCall.vertexCount   = vertexCount;
        commandBufferWrapper->drawCall.instanceCount = instanceCount;
        commandBufferWrapper->drawCall.firstVertex   = firstVertex;
        commandBufferWrapper->drawCall.firstInstance = firstInstance;
    }
}

void MetalWrapper::cmdBeginRenderPass(
    FvCommandBuffer commandBuffer,
    const FvRenderPassBeginInfo *renderPassInfo) {
    // Get command buffer wrapper
    CommandBufferWrapper *commandBufferWrapper = nullptr;

    const Handle *handle = (const Handle *)commandBuffer;

    if (handle != nullptr) {
        commandBufferWrapper = commandBuffers.get(*handle);
    }

    if (renderPassInfo == nullptr || commandBufferWrapper == nullptr) {
        return;
    }

    // Get framebuffer
    FramebufferWrapper *framebufferWrapper = nullptr;
    handle = (const Handle *)renderPassInfo->framebuffer;

    if (handle != nullptr) {
        framebufferWrapper = framebuffers.get(*handle);
    }

    if (framebufferWrapper == nullptr) {
        return;
    }

    // Store clear values
    for (uint32_t i = 0; i < renderPassInfo->clearValueCount; ++i) {
        commandBufferWrapper->clearValues.push_back(
            renderPassInfo->clearValues[i]);
    }

    // Store texture attachments
    commandBufferWrapper->attachments = framebufferWrapper->attachments;
}

void MetalWrapper::cmdEndRenderPass(FvCommandBuffer commandBuffer) {
    // Get command buffer wrapper
    CommandBufferWrapper *commandBufferWrapper = nullptr;

    const Handle *handle = (const Handle *)commandBuffer;

    if (handle != nullptr) {
        commandBufferWrapper = commandBuffers.get(*handle);

        if (commandBufferWrapper != nullptr) {
            commandBufferWrapper->readyForSubmit = true;
        }
    }
}

FvResult MetalWrapper::commandBufferCreate(FvCommandBuffer *commandBuffer,
                                           FvCommandPool commandPool) {
    // Get command pool to create command buffer from
    id<MTLCommandQueue> commandQueue = nil;
    const Handle *handle             = (const Handle *)commandPool;

    if (handle != nullptr) {
        id<MTLCommandQueue> *tmp = commandQueues.get(*handle);

        // Destroy command queue
        if (tmp != nullptr) {
            commandQueue = *tmp;
        }
    }

    if (commandBuffer == nullptr || commandQueue == nil) {
        return FV_RESULT_FAILURE;
    }

    CommandBufferWrapper commandBufferWrapper;
    commandBufferWrapper.commandQueue   = commandQueue;
    commandBufferWrapper.readyForSubmit = false;

    // Store command buffer and return handle
    handle = commandBuffers.add(commandBufferWrapper);

    if (handle != nullptr) {
        *commandBuffer = (FvCommandBuffer)handle;
    } else {
        return FV_RESULT_FAILURE;
    }

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::commandBufferBegin(FvCommandBuffer commandBuffer) {
    // Clear recorded commands
}

FvResult MetalWrapper::commandBufferEnd(FvCommandBuffer commandBuffer) {
    // Get command buffer wrapper
    CommandBufferWrapper *commandBufferWrapper = nullptr;

    const Handle *handle = (const Handle *)commandBuffer;

    if (handle != nullptr) {
        commandBufferWrapper = commandBuffers.get(*handle);
    }

    if (commandBufferWrapper == nullptr) {
        return FV_RESULT_FAILURE;
    }

    if (commandBufferWrapper->readyForSubmit == false) {
        // Render pass not ended with 'fvCmdEndRenderPass'
        return FV_RESULT_FAILURE;
    }

    return FV_RESULT_SUCCESS;
}

FvResult
MetalWrapper::commandPoolCreate(FvCommandPool *commandPool,
                                const FvCommandPoolCreateInfo *createInfo) {
    if (createInfo == nullptr || commandPool == nullptr) {
        return FV_RESULT_FAILURE;
    }

    // Create command queue (MTL) / command pool (Fever)
    id<MTLCommandQueue> commandQueue = [device newCommandQueue];

    if (commandQueue == nil) {
        return FV_RESULT_FAILURE;
    }

    // Store command queue and return handle
    const Handle *handle = commandQueues.add(commandQueue);

    if (handle != nullptr) {
        *commandPool = (FvCommandPool)handle;
    } else {
        return FV_RESULT_FAILURE;
    }

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::commandPoolDestroy(FvCommandPool commandPool) {
    const Handle *handle = (const Handle *)commandPool;

    if (handle != nullptr) {
        id<MTLCommandQueue> *commandQueue = commandQueues.get(*handle);

        // Destroy command queue
        if (commandQueue != nullptr) {
            *commandQueue = nil;
        }

        commandQueues.remove(*handle);
    }
}

FvResult
MetalWrapper::framebufferCreate(FvFramebuffer *framebuffer,
                                const FvFramebufferCreateInfo *createInfo) {
    if (createInfo == nullptr || framebuffer == nullptr) {
        return FV_RESULT_FAILURE;
    }

    FramebufferWrapper framebufferWrapper;
    for (uint32_t i = 0; i < createInfo->attachmentCount; ++i) {
        // Get texture of image view attachment and store in the frambuffer
        // wrapper
        const Handle *handle = (const Handle *)createInfo->attachments[i];

        if (handle != nullptr) {
            id<MTLTexture> *texture = textureViews.get(*handle);

            if (texture != nullptr) {
                framebufferWrapper.attachments.push_back(*texture);
            }
        }
    }

    // Store framebuffer and return handle
    const Handle *handle = framebuffers.add(framebufferWrapper);

    if (handle != nullptr) {
        *framebuffer = (FvFramebuffer)handle;
    } else {
        return FV_RESULT_FAILURE;
    }

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::framebufferDestroy(FvFramebuffer framebuffer) {
    const Handle *handle = (const Handle *)framebuffer;

    if (handle != nullptr) {
        framebuffers.remove(*handle);
    }
}

FvResult
MetalWrapper::imageViewCreate(FvImageView *imageView,
                              const FvImageViewCreateInfo *createInfo) {
    if (createInfo == nullptr || imageView == nullptr) {
        return FV_RESULT_FAILURE;
    }

    // Get texture this texture view will be sharing a storage allocation with.
    id<MTLTexture> texture = nil;
    const Handle *handle   = (const Handle *)createInfo->image;

    if (handle != nullptr) {
        id<MTLTexture> *texturePtr = textures.get(*handle);

        if (texturePtr != nullptr) {
            texture = *texturePtr;
        }
    }

    if (texture == nil) {
        return FV_RESULT_FAILURE;
    }

    id<MTLTexture> textureView = [texture
        newTextureViewWithPixelFormat:toMtlPixelFormat(createInfo->format)];

    // Store texture view and return handle
    handle = textureViews.add(textureView);

    if (handle != nullptr) {
        *imageView = (FvImageView)handle;
    } else {
        return FV_RESULT_FAILURE;
    }

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::imageViewDestroy(FvImageView imageView) {
    const Handle *handle = (const Handle *)imageView;

    if (handle != nullptr) {
        id<MTLTexture> *textureView = textureViews.get(*handle);

        // Destroy texture view
        if (textureView != nullptr) {
            *textureView = nil;
        }

        textureViews.remove(*handle);
    }
}

FvResult MetalWrapper::imageCreate(FvImage *image,
                                   const FvImageCreateInfo *createInfo) {
    if (createInfo == nullptr || image == nullptr) {
        return FV_RESULT_FAILURE;
    }

    MTLTextureDescriptor *textureDesc;

    // Determine basic dimensionality of image and create descriptor
    if (createInfo->imageType == FV_IMAGE_TYPE_1D ||
        createInfo->imageType == FV_IMAGE_TYPE_2D) {
        textureDesc = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:toMtlPixelFormat(
                                                   createInfo->format)
                                         width:createInfo->extent.width
                                        height:createInfo->extent.height
                                     mipmapped:createInfo->numMipmapLevels > 1
                                                   ? YES
                                                   : NO];

    } else {
        textureDesc = [MTLTextureDescriptor
            textureCubeDescriptorWithPixelFormat:toMtlPixelFormat(
                                                     createInfo->format)
                                            size:createInfo->extent.width
                                       mipmapped:createInfo->numMipmapLevels > 1
                                                     ? YES
                                                     : NO];
    }

    if (textureDesc == NULL) {
        return FV_RESULT_FAILURE;
    }

    // Setup descriptor
    textureDesc.pixelFormat      = toMtlPixelFormat(createInfo->format);
    textureDesc.width            = createInfo->extent.width;
    textureDesc.height           = createInfo->extent.height;
    textureDesc.depth            = createInfo->extent.depth;
    textureDesc.mipmapLevelCount = createInfo->numMipmapLevels;
    textureDesc.sampleCount      = toMtlSampleCount(createInfo->numSamples);
    textureDesc.arrayLength      = createInfo->arrayLayers;
    textureDesc.usage            = toMtlTextureUsage(createInfo->usage);

    // Create texture
    id<MTLTexture> texture = [device newTextureWithDescriptor:textureDesc];

    if (texture == nil) {
        return FV_RESULT_FAILURE;
    }

    // Store texture and return handle
    const Handle *handle = textures.add(texture);

    if (handle != nullptr) {
        *image = (FvImage)handle;
    } else {
        return FV_RESULT_FAILURE;
    }

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::imageDestroy(FvImage image) {
    const Handle *handle = (const Handle *)image;

    if (handle != nullptr) {
        id<MTLTexture> *texture = textures.get(*handle);

        // Destroy texture
        if (texture != nullptr) {
            *texture = nil;
        }

        textures.remove(*handle);
    }
}

FvResult MetalWrapper::graphicsPipelineCreate(
    FvGraphicsPipeline *graphicsPipeline,
    const FvGraphicsPipelineCreateInfo *createInfo) {
    // TODO: Clean up, put into separate functions, deal with optional fields
    // early
    FvResult result = FV_RESULT_FAILURE;

    if (graphicsPipeline != nullptr && createInfo != nullptr) {
        // Create GraphicsPipelineWrapper
        GraphicsPipelineWrapper graphicsPipelineWrapper;
        graphicsPipelineWrapper.renderPass          = NULL;
        graphicsPipelineWrapper.cullMode            = MTLCullModeNone;
        graphicsPipelineWrapper.windingOrder        = MTLWindingClockwise;
        graphicsPipelineWrapper.depthClipMode       = MTLDepthClipModeClip;
        graphicsPipelineWrapper.depthStencilState   = nil;
        graphicsPipelineWrapper.renderPipelineState = nil;

        // Get subpass to use from render pass and index
        Handle *renderPassHandle = (Handle *)createInfo->renderPass;

        if (renderPassHandle != nullptr) {
            RenderPassWrapper *renderPassWrapper =
                renderPasses.get(*renderPassHandle);

            if (renderPassWrapper != nullptr) {
                SubpassWrapper subpassWrapper =
                    renderPassWrapper->subpasses[createInfo->subpass];

                // Copy attachment descriptions to graphics pipeline from
                // subpass
                graphicsPipelineWrapper.inputAttachments =
                    subpassWrapper.inputAttachments;
                graphicsPipelineWrapper.colorAttachments =
                    subpassWrapper.colorAttachments;
                graphicsPipelineWrapper.depthStencilAttachment =
                    subpassWrapper.depthStencilAttachment;

                // Fill out shader functions for Metal pipeline descriptor
                MTLRenderPipelineDescriptor *mtlPipelineDescriptor =
                    subpassWrapper.mtlPipelineDescriptor;

                if (createInfo->stages != nullptr) {
                    // Loop thru shader stages and assign functions to
                    // mtlPipelineDescriptor
                    for (uint32_t i = 0; i < createInfo->stageCount; ++i) {
                        FvPipelineShaderStageDescription shaderStage =
                            createInfo->stages[i];

                        Handle *shaderModuleHandle =
                            (Handle *)shaderStage.shaderModule;
                        if (shaderModuleHandle != nullptr) {
                            id<MTLLibrary> *library =
                                libraries.get(*shaderModuleHandle);

                            if (library != nullptr) {
                                id<MTLFunction> func = [*library
                                    newFunctionWithName:
                                        @(shaderStage.entryFunctionName)];

                                if (func != nil) {
                                    switch (shaderStage.stage) {
                                    case FV_SHADER_STAGE_VERTEX:
                                        mtlPipelineDescriptor.vertexFunction =
                                            func;
                                        result = FV_RESULT_SUCCESS;
                                        break;
                                    case FV_SHADER_STAGE_FRAGMENT:
                                        mtlPipelineDescriptor.fragmentFunction =
                                            func;
                                        result = FV_RESULT_SUCCESS;
                                        break;
                                    default:
                                        break;
                                    }
                                } else {
                                    printf("Failed to find function with name "
                                           "'%s' in shader module.\n",
                                           shaderStage.entryFunctionName);
                                }
                            }
                        }
                    }
                }

                // Create color blend states for ColorBlendAttachmentState
                if (createInfo->colorBlendStateDescription != nullptr &&
                    createInfo->colorBlendStateDescription->attachments !=
                        nullptr) {
                    FvPipelineColorBlendStateDescription colorBlendState =
                        *createInfo->colorBlendStateDescription;

                    // Loop through color attachments and set color blend states
                    for (uint32_t i = 0; i < colorBlendState.attachmentCount;
                         ++i) {
                        mtlPipelineDescriptor.colorAttachments[i]
                            .blendingEnabled = toObjCBool(
                            colorBlendState.attachments[i].blendEnable);
                        mtlPipelineDescriptor.colorAttachments[i]
                            .sourceRGBBlendFactor = toMtlBlendFactor(
                            colorBlendState.attachments[i].srcColorBlendFactor);
                        mtlPipelineDescriptor.colorAttachments[i]
                            .destinationRGBBlendFactor = toMtlBlendFactor(
                            colorBlendState.attachments[i].dstColorBlendFactor);
                        mtlPipelineDescriptor.colorAttachments[i]
                            .rgbBlendOperation = toMtlBlendOperation(
                            colorBlendState.attachments[i].colorBlendOp);
                        mtlPipelineDescriptor.colorAttachments[i]
                            .sourceAlphaBlendFactor = toMtlBlendFactor(
                            colorBlendState.attachments[i].srcAlphaBlendFactor);
                        mtlPipelineDescriptor.colorAttachments[i]
                            .destinationAlphaBlendFactor = toMtlBlendFactor(
                            colorBlendState.attachments[i].dstAlphaBlendFactor);
                        mtlPipelineDescriptor.colorAttachments[i]
                            .alphaBlendOperation = toMtlBlendOperation(
                            colorBlendState.attachments[i].alphaBlendOp);
                    }
                }

                // We've now done all the work required to make a
                // MTLRenderPipelineState object:
                NSError *err                                      = nil;
                id<MTLRenderPipelineState> mtlRenderPipelineState = [device
                    newRenderPipelineStateWithDescriptor:mtlPipelineDescriptor
                                                   error:&err];

                if (mtlRenderPipelineState != nil) {
                    graphicsPipelineWrapper.renderPipelineState =
                        mtlRenderPipelineState;
                } else {
                    NSString *errString =
                        [NSString stringWithFormat:@"%@", err];

                    printf(
                        "Failed to create render pipeline state: %s\n",
                        [errString cStringUsingEncoding:NSUTF8StringEncoding]);

                    result = FV_RESULT_FAILURE;
                }

                // Create depth stencil states from DepthStencilDescription
                if (createInfo->depthStencilDescription != nullptr) {
                    FvPipelineDepthStencilStateDescription depthStencilDesc =
                        *createInfo->depthStencilDescription;

                    MTLDepthStencilDescriptor *mtlDepthStencilDesc =
                        [[MTLDepthStencilDescriptor alloc] init];
                    mtlDepthStencilDesc.depthCompareFunction =
                        toMtlCompareFunction(depthStencilDesc.depthCompareFunc);
                    mtlDepthStencilDesc.depthWriteEnabled =
                        toObjCBool(depthStencilDesc.depthWriteEnable);

                    mtlDepthStencilDesc.backFaceStencil
                        .stencilFailureOperation = toMtlStencilOperation(
                        depthStencilDesc.backFaceStencil.stencilFailOp);
                    mtlDepthStencilDesc.backFaceStencil.depthFailureOperation =
                        toMtlStencilOperation(
                            depthStencilDesc.backFaceStencil.depthFailOp);
                    mtlDepthStencilDesc.backFaceStencil
                        .depthStencilPassOperation = toMtlStencilOperation(
                        depthStencilDesc.backFaceStencil.depthStencilPassOp);
                    mtlDepthStencilDesc.backFaceStencil.stencilCompareFunction =
                        toMtlCompareFunction(depthStencilDesc.backFaceStencil
                                                 .stencilCompareFunc);
                    mtlDepthStencilDesc.backFaceStencil.readMask =
                        depthStencilDesc.backFaceStencil.readMask;
                    mtlDepthStencilDesc.backFaceStencil.writeMask =
                        depthStencilDesc.backFaceStencil.writeMask;

                    mtlDepthStencilDesc.frontFaceStencil
                        .stencilFailureOperation = toMtlStencilOperation(
                        depthStencilDesc.frontFaceStencil.stencilFailOp);
                    mtlDepthStencilDesc.frontFaceStencil.depthFailureOperation =
                        toMtlStencilOperation(
                            depthStencilDesc.frontFaceStencil.depthFailOp);
                    mtlDepthStencilDesc.frontFaceStencil
                        .depthStencilPassOperation = toMtlStencilOperation(
                        depthStencilDesc.frontFaceStencil.depthStencilPassOp);
                    mtlDepthStencilDesc.frontFaceStencil
                        .stencilCompareFunction = toMtlCompareFunction(
                        depthStencilDesc.frontFaceStencil.stencilCompareFunc);
                    mtlDepthStencilDesc.frontFaceStencil.readMask =
                        depthStencilDesc.frontFaceStencil.readMask;
                    mtlDepthStencilDesc.frontFaceStencil.writeMask =
                        depthStencilDesc.frontFaceStencil.writeMask;

                    id<MTLDepthStencilState> depthStencilState = [device
                        newDepthStencilStateWithDescriptor:mtlDepthStencilDesc];

                    if (mtlRenderPipelineState != nil) {
                        graphicsPipelineWrapper.depthStencilState =
                            depthStencilState;
                    } else {
                        printf("Failed to create depth stencil state.\n");

                        result = FV_RESULT_FAILURE;
                    }
                }

                // Fill out rasterizer info
                if (createInfo->rasterizerDescription != nullptr) {
                    FvPipelineRasterizerDescription rasterizer =
                        *createInfo->rasterizerDescription;

                    graphicsPipelineWrapper.cullMode =
                        toMtlCullMode(rasterizer.cullMode);
                    graphicsPipelineWrapper.windingOrder =
                        toMtlWindingOrder(rasterizer.frontFacing);
                    graphicsPipelineWrapper.depthClipMode =
                        rasterizer.depthClampEnable ? MTLDepthClipModeClamp
                                                    : MTLDepthClipModeClip;
                } else {
                    result = FV_RESULT_FAILURE;
                }

                // Fill out viewport and scissor info
                if (createInfo->viewportDescription != nullptr) {
                    FvPipelineViewportDescription viewportDescription =
                        *createInfo->viewportDescription;

                    graphicsPipelineWrapper.viewport.originX =
                        viewportDescription.viewport.x;
                    graphicsPipelineWrapper.viewport.originY =
                        viewportDescription.viewport.y;
                    graphicsPipelineWrapper.viewport.width =
                        viewportDescription.viewport.width;
                    graphicsPipelineWrapper.viewport.height =
                        viewportDescription.viewport.height;
                    graphicsPipelineWrapper.viewport.znear =
                        viewportDescription.viewport.minDepth;
                    graphicsPipelineWrapper.viewport.zfar =
                        viewportDescription.viewport.maxDepth;

                    graphicsPipelineWrapper.scissor.x =
                        viewportDescription.scissor.offset.x;
                    graphicsPipelineWrapper.scissor.y =
                        viewportDescription.scissor.offset.y;
                    graphicsPipelineWrapper.scissor.width =
                        viewportDescription.scissor.extent.width;
                    graphicsPipelineWrapper.scissor.height =
                        viewportDescription.scissor.extent.height;
                } else {
                    result = FV_RESULT_FAILURE;
                }
            }
        }

        if (result == FV_RESULT_SUCCESS) {
            // Store graphics pipeline wrapper and return handle as graphics
            // pipeline
            const Handle *handle =
                graphicsPipelines.add(graphicsPipelineWrapper);

            if (handle != nullptr) {
                *graphicsPipeline = (FvGraphicsPipeline)handle;

            } else {
                result = FV_RESULT_FAILURE;
            }
        }
    }

    return result;
}

void MetalWrapper::graphicsPipelineDestroy(
    FvGraphicsPipeline graphicsPipeline) {
    const Handle *handle = (const Handle *)graphicsPipeline;

    if (handle != nullptr) {
        GraphicsPipelineWrapper *pipeline = graphicsPipelines.get(*handle);

        // Destroy pipeline
        if (pipeline != nullptr) {
            pipeline->depthStencilState   = nil;
            pipeline->renderPipelineState = nil;
        }

        graphicsPipelines.remove(*handle);
    }
}

FvResult
MetalWrapper::renderPassCreate(FvRenderPass *renderPass,
                               const FvRenderPassCreateInfo *createInfo) {
    FvResult result = FV_RESULT_FAILURE;

    // Aggregate structure to hold subpass information
    RenderPassWrapper renderPassWrapper;

    if (renderPass != nullptr && createInfo != nullptr) {
        // For each subpass
        for (uint32_t i = 0; i < createInfo->subpassCount; ++i) {
            FvSubpassDescription subpassDescription = createInfo->subpasses[i];

            // Create subpass wrapper to store subpass information, we cannot
            // complete the analogous Metal structures until we get more
            // information (such as texture and clear values), so we store them
            // for later completion.
            SubpassWrapper subpassWrapper;
            subpassWrapper.mtlRenderPass = [MTLRenderPassDescriptor new];
            subpassWrapper.mtlPipelineDescriptor =
                [MTLRenderPipelineDescriptor new];

            MTLRenderPassDescriptor *mtlRenderPassDescriptor =
                subpassWrapper.mtlRenderPass;

            MTLRenderPipelineDescriptor *mtlRenderPipelineDescriptor =
                subpassWrapper.mtlPipelineDescriptor;

            // Fill out fields we can (load action, store action, format)
            // Color attachments
            for (uint32_t j = 0; j < subpassDescription.colorAttachmentCount;
                 ++j) {
                uint32_t colorAttachmentIndex =
                    subpassDescription.colorAttachments[j].attachment;

                FvAttachmentDescription colorAttachment =
                    createInfo->attachments[colorAttachmentIndex];

                mtlRenderPassDescriptor.colorAttachments[j].loadAction =
                    toMtlLoadAction(colorAttachment.loadOp);
                mtlRenderPassDescriptor.colorAttachments[j].storeAction =
                    toMtlStoreAction(colorAttachment.storeOp);

                mtlRenderPipelineDescriptor.colorAttachments[j].pixelFormat =
                    toMtlPixelFormat(colorAttachment.format);
            }

            // Depth/Stencil attachment
            if (subpassDescription.depthStencilAttachment != nullptr) {
                uint32_t depthStencilAttachmentIndex =
                    subpassDescription.depthStencilAttachment->attachment;

                FvAttachmentDescription depthStencilAttachment =
                    createInfo->attachments[depthStencilAttachmentIndex];

                // Depth
                mtlRenderPassDescriptor.depthAttachment.loadAction =
                    toMtlLoadAction(depthStencilAttachment.loadOp);
                mtlRenderPassDescriptor.depthAttachment.storeAction =
                    toMtlStoreAction(depthStencilAttachment.storeOp);

                mtlRenderPipelineDescriptor.depthAttachmentPixelFormat =
                    toMtlPixelFormat(depthStencilAttachment.format);

                // Stencil
                mtlRenderPassDescriptor.stencilAttachment.loadAction =
                    toMtlLoadAction(depthStencilAttachment.stencilLoadOp);
                mtlRenderPassDescriptor.stencilAttachment.storeAction =
                    toMtlStoreAction(depthStencilAttachment.stencilStoreOp);

                mtlRenderPipelineDescriptor.stencilAttachmentPixelFormat =
                    toMtlPixelFormat(depthStencilAttachment.format);
            }

            // Store attachment references
            for (uint32_t j = 0; j < subpassDescription.inputAttachmentCount;
                 ++j) {
                subpassWrapper.inputAttachments.push_back(
                    subpassDescription.inputAttachments[j]);
            }

            for (uint32_t j = 0; j < subpassDescription.colorAttachmentCount;
                 ++j) {
                subpassWrapper.colorAttachments.push_back(
                    subpassDescription.colorAttachments[j]);
            }

            if (subpassDescription.depthStencilAttachment != nullptr) {
                subpassWrapper.depthStencilAttachment.push_back(
                    *subpassDescription.depthStencilAttachment);
            }

            // Add subpass to renderPass aggregate
            renderPassWrapper.subpasses.push_back(subpassWrapper);
        }

        // Store render pass wrapper and return handle as render pass
        const Handle *handle = renderPasses.add(renderPassWrapper);

        if (handle != nullptr) {
            *renderPass = (FvRenderPass)handle;

            result = FV_RESULT_SUCCESS;
        }
    }

    return result;
}

void MetalWrapper::renderPassDestroy(FvRenderPass renderPass) {
    const Handle *handle = (const Handle *)renderPass;

    if (handle != nullptr) {
        renderPasses.remove(*handle);
    }
}

FvResult MetalWrapper::pipelineLayoutCreate(
    FvPipelineLayout *layout, const FvPipelineLayoutCreateInfo *createInfo) {
    // TODO: Unused at this point
    return FV_RESULT_SUCCESS;
}

void MetalWrapper::pipelineLayoutDestroy(FvPipelineLayout layout) {}

FvResult
MetalWrapper::shaderModuleCreate(FvShaderModule *shaderModule,
                                 const FvShaderModuleCreateInfo *createInfo) {
    FvResult result = FV_RESULT_FAILURE;

    if (shaderModule != nullptr && createInfo != nullptr &&
        createInfo->data != nullptr) {
        MTLCompileOptions *options = [MTLCompileOptions new];

        NSError *error;
        id<MTLLibrary> library =
            [device newLibraryWithSource:@((const char *)createInfo->data)
                                 options:options
                                   error:&error];

        if (error == nil) {
            const Handle *handle = libraries.add(library);

            if (handle != nullptr) {
                *shaderModule = (FvShaderModule)handle;

                result = FV_RESULT_SUCCESS;
            }
        } else {
            NSString *errString = [NSString
                stringWithFormat:@"%@",
                                 [[error userInfo]
                                     objectForKey:@"NSLocalizedDescription"]];

            printf("%s\n",
                   [errString cStringUsingEncoding:NSUTF8StringEncoding]);
        }
    }

    return result;
}

void MetalWrapper::shaderModuleDestroy(FvShaderModule shaderModule) {
    const Handle *handle = (const Handle *)shaderModule;

    if (handle != nullptr) {
        // Destroy library
        id<MTLLibrary> *lib = libraries.get(*handle);
        if (lib != nullptr) {
            *lib = nil;
        }

        // Remove from handle manager
        libraries.remove(*handle);
    }
}

MTLLoadAction MetalWrapper::toMtlLoadAction(FvLoadOp loadOp) {
    MTLLoadAction loadAction = MTLLoadActionDontCare;

    switch (loadOp) {
    case FV_LOAD_OP_DONT_CARE:
        loadAction = MTLLoadActionDontCare;
        break;
    case FV_LOAD_OP_LOAD:
        loadAction = MTLLoadActionLoad;
        break;
    case FV_LOAD_OP_CLEAR:
        loadAction = MTLLoadActionClear;
        break;
    default:
        break;
    };

    return loadAction;
}

MTLStoreAction MetalWrapper::toMtlStoreAction(FvStoreOp storeOp) {
    MTLStoreAction storeAction = MTLStoreActionDontCare;

    switch (storeOp) {
    case FV_STORE_OP_DONT_CARE:
        storeAction = MTLStoreActionDontCare;
        break;
    case FV_STORE_OP_STORE:
        storeAction = MTLStoreActionStore;
        break;
    default:
        break;
    };

    return storeAction;
}

MTLPixelFormat MetalWrapper::toMtlPixelFormat(FvFormat format) {
    MTLPixelFormat pixelFormat = MTLPixelFormatInvalid;

    switch (format) {
    case FV_FORMAT_RGBA8UNORM:
        pixelFormat = MTLPixelFormatRGBA8Unorm;
        break;
    case FV_FORMAT_RGBA16FLOAT:
        pixelFormat = MTLPixelFormatRGBA16Float;
        break;
    case FV_FORMAT_DEPTH32FLOAT_STENCIL8:
        pixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        break;
    case FV_FORMAT_BGRA8UNORM:
        pixelFormat = MTLPixelFormatBGRA8Unorm;
        break;
    case FV_FORMAT_R32_SFLOAT:
        pixelFormat = MTLPixelFormatR32Float;
        break;
    case FV_FORMAT_R32G32_SFLOAT:
        pixelFormat = MTLPixelFormatRG32Float;
        break;
    case FV_FORMAT_R32G32B32A32_SFLOAT:
        pixelFormat = MTLPixelFormatRGBA32Float;
        break;
    default:
        break;
    };

    return pixelFormat;
}

MTLBlendFactor MetalWrapper::toMtlBlendFactor(FvBlendFactor factor) {
    MTLBlendFactor blendFactor = MTLBlendFactorZero;

    switch (factor) {
    case FV_BLEND_FACTOR_ZERO:
        blendFactor = MTLBlendFactorZero;
        break;
    case FV_BLEND_FACTOR_ONE:
        blendFactor = MTLBlendFactorOne;
        break;
    case FV_BLEND_FACTOR_SOURCE_COLOR:
        blendFactor = MTLBlendFactorSourceColor;
        break;
    case FV_BLEND_FACTOR_ONE_MINUS_SOURCE_COLOR:
        blendFactor = MTLBlendFactorOneMinusSourceColor;
        break;
    case FV_BLEND_FACTOR_SOURCE_ALPHA:
        blendFactor = MTLBlendFactorSourceAlpha;
        break;
    case FV_BLEND_FACTOR_ONE_MINUS_SOURCE_ALPHA:
        blendFactor = MTLBlendFactorOneMinusSourceAlpha;
        break;
    case FV_BLEND_FACTOR_DST_COLOR:
        blendFactor = MTLBlendFactorDestinationColor;
        break;
    case FV_BLEND_FACTOR_ONE_MINUS_DST_COLOR:
        blendFactor = MTLBlendFactorOneMinusDestinationColor;
        break;
    case FV_BLEND_FACTOR_DST_ALPHA:
        blendFactor = MTLBlendFactorDestinationAlpha;
        break;
    case FV_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
        blendFactor = MTLBlendFactorOneMinusDestinationAlpha;
        break;
    case FV_BLEND_FACTOR_SOURCE_ALPHA_SATURATED:
        blendFactor = MTLBlendFactorSourceAlphaSaturated;
        break;
    case FV_BLEND_FACTOR_BLEND_COLOR:
        blendFactor = MTLBlendFactorBlendColor;
        break;
    case FV_BLEND_FACTOR_ONE_MINUS_BLEND_COLOR:
        blendFactor = MTLBlendFactorOneMinusBlendColor;
        break;
    case FV_BLEND_FACTOR_BLEND_ALPHA:
        blendFactor = MTLBlendFactorBlendAlpha;
        break;
    case FV_BLEND_FACTOR_ONE_MINUS_BLEND_ALPHA:
        blendFactor = MTLBlendFactorOneMinusBlendAlpha;
        break;
    case FV_BLEND_FACTOR_SOURCE1_COLOR:
        blendFactor = MTLBlendFactorSource1Color;
        break;
    case FV_BLEND_FACTOR_ONE_MINUS_SOURCE1_COLOR:
        blendFactor = MTLBlendFactorOneMinusSource1Color;
        break;
    case FV_BLEND_FACTOR_SOURCE1_ALPHA:
        blendFactor = MTLBlendFactorSource1Alpha;
        break;
    case FV_BLEND_FACTOR_ONE_MINUS_SOURCE1_ALPHA:
        blendFactor = MTLBlendFactorOneMinusSource1Alpha;
        break;
    default:
        break;
    }

    return blendFactor;
}

MTLBlendOperation MetalWrapper::toMtlBlendOperation(FvBlendOp op) {
    MTLBlendOperation blendOperation = MTLBlendOperationAdd;

    switch (op) {
    FV_BLEND_OP_ADD:
        blendOperation = MTLBlendOperationAdd;
        break;
    FV_BLEND_OP_SUBTRACT:
        blendOperation = MTLBlendOperationSubtract;
        break;
    FV_BLEND_OP_REVERSE_SUBTRACT:
        blendOperation = MTLBlendOperationReverseSubtract;
        break;
    FV_BLEND_OP_MIN:
        blendOperation = MTLBlendOperationMin;
        break;
    FV_BLEND_OP_MAX:
        blendOperation = MTLBlendOperationMax;
        break;
    default:
        break;
    }

    return blendOperation;
}

BOOL MetalWrapper::toObjCBool(bool b) { return (b ? YES : NO); }

MTLWinding MetalWrapper::toMtlWindingOrder(FvWindingOrder winding) {
    MTLWinding windingOrder = MTLWindingClockwise;

    switch (winding) {
    case FV_WINDING_ORDER_CLOCKWISE:
        windingOrder = MTLWindingClockwise;
        break;
    case FV_WINDING_ORDER_COUNTER_CLOCKWISE:
        windingOrder = MTLWindingCounterClockwise;
        break;
    default:
        break;
    }

    return windingOrder;
}

MTLCullMode MetalWrapper::toMtlCullMode(FvCullMode cull) {
    MTLCullMode cullMode = MTLCullModeNone;

    switch (cull) {
    case FV_CULL_MODE_NONE:
        cullMode = MTLCullModeNone;
        break;
    case FV_CULL_MODE_FRONT:
        cullMode = MTLCullModeFront;
        break;
    case FV_CULL_MODE_BACK:
        cullMode = MTLCullModeBack;
        break;
    default:
        break;
    }

    return cullMode;
}

MTLStencilOperation MetalWrapper::toMtlStencilOperation(FvStencilOp stencil) {
    MTLStencilOperation stencilOperation = MTLStencilOperationKeep;

    switch (stencil) {
    case FV_STENCIL_OP_KEEP:
        stencilOperation = MTLStencilOperationKeep;
        break;
    case FV_STENCIL_OP_ZERO:
        stencilOperation = MTLStencilOperationZero;
        break;
    case FV_STENCIL_OP_REPLACE:
        stencilOperation = MTLStencilOperationReplace;
        break;
    case FV_STENCIL_OP_INCREMENT_CLAMP:
        stencilOperation = MTLStencilOperationIncrementClamp;
        break;
    case FV_STENCIL_OP_DECREMENT_CLAMP:
        stencilOperation = MTLStencilOperationDecrementClamp;
        break;
    case FV_STENCIL_OP_INVERT:
        stencilOperation = MTLStencilOperationInvert;
        break;
    case FV_STENCIL_OP_INCREMENT_WRAP:
        stencilOperation = MTLStencilOperationIncrementWrap;
        break;
    case FV_STENCIL_OP_DECREMENT_WRAP:
        stencilOperation = MTLStencilOperationDecrementWrap;
        break;
    default:
        break;
    }

    return stencilOperation;
}

MTLCompareFunction MetalWrapper::toMtlCompareFunction(FvCompareFunc compare) {
    MTLCompareFunction compareFunction = MTLCompareFunctionAlways;

    switch (compare) {

    case FV_COMPARE_FUNC_NEVER:
        compareFunction = MTLCompareFunctionNever;
        break;
    case FV_COMPARE_FUNC_LESS:
        compareFunction = MTLCompareFunctionLess;
        break;
    case FV_COMPARE_FUNC_EQUAL:
        compareFunction = MTLCompareFunctionEqual;
        break;
    case FV_COMPARE_FUNC_LESS_EQUAL:
        compareFunction = MTLCompareFunctionLessEqual;
        break;
    case FV_COMPARE_FUNC_NOT_EQUAL:
        compareFunction = MTLCompareFunctionNotEqual;
        break;
    case FV_COMPARE_FUNC_GREATER:
        compareFunction = MTLCompareFunctionGreater;
        break;
    case FV_COMPARE_FUNC_GREATER_EQUAL:
        compareFunction = MTLCompareFunctionGreaterEqual;
        break;
    case FV_COMPARE_FUNC_ALWAYS:
        compareFunction = MTLCompareFunctionAlways;
        break;
    default:
        break;
    }

    return compareFunction;
}

uint32_t MetalWrapper::toMtlSampleCount(FvSampleCount samples) {
    uint32_t sampleCount = 1;

    switch (samples) {
    case FV_SAMPLE_COUNT_1:
        sampleCount = 1;
        break;
    case FV_SAMPLE_COUNT_2:
        sampleCount = 2;
        break;
    case FV_SAMPLE_COUNT_4:
        sampleCount = 4;
        break;
    case FV_SAMPLE_COUNT_8:
        sampleCount = 8;
        break;
    case FV_SAMPLE_COUNT_16:
        sampleCount = 16;
        break;
    case FV_SAMPLE_COUNT_32:
        sampleCount = 32;
        break;
    case FV_SAMPLE_COUNT_64:
        sampleCount = 64;
        break;
    default:
        break;
    }

    return sampleCount;
}

MTLTextureUsage MetalWrapper::toMtlTextureUsage(FvImageUsage usage) {
    MTLTextureUsage textureUsage = MTLTextureUsageUnknown;

    if (usage & FV_IMAGE_USAGE_RENDER_TARGET) {
        textureUsage |= MTLTextureUsageRenderTarget;
    }
    if (usage & FV_IMAGE_USAGE_SHADER_READ) {
        textureUsage |= MTLTextureUsageShaderRead;
    }
    if (usage & FV_IMAGE_USAGE_SHADER_WRITE) {
        textureUsage |= MTLTextureUsageShaderWrite;
    }
    if (usage & FV_IMAGE_USAGE_IMAGE_VIEW) {
        textureUsage |= MTLTextureUsagePixelFormatView;
    }

    return textureUsage;
}
}
