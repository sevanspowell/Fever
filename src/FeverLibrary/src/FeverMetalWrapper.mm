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

FvResult MetalWrapper::graphicsPipelineCreate(
    FvGraphicsPipeline *graphicsPipeline,
    const FvGraphicsPipelineCreateInfo *createInfo) {
    FvResult result = FV_RESULT_FAILURE;

    return result;
}

void MetalWrapper::graphicsPipelineDestroy(
    FvGraphicsPipeline graphicsPipeline) {}

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

    if (shaderModule != nullptr && createInfo != nullptr) {
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
    FV_FORMAT_RGBA8UNORM:
        pixelFormat = MTLPixelFormatRGBA8Unorm;
        break;
    FV_FORMAT_RGBA16FLOAT:
        pixelFormat = MTLPixelFormatRGBA16Float;
        break;
    FV_FORMAT_DEPTH32FLOAT_STENCIL8:
        pixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        break;
    FV_FORMAT_BGRA8UNORM:
        pixelFormat = MTLPixelFormatBGRA8Unorm;
        break;
    FV_FORMAT_R32_SFLOAT:
        pixelFormat = MTLPixelFormatR32Float;
        break;
    FV_FORMAT_R32G32_SFLOAT:
        pixelFormat = MTLPixelFormatRG32Float;
        break;
    FV_FORMAT_R32G32B32A32_SFLOAT:
        pixelFormat = MTLPixelFormatRGBA32Float;
        break;
    default:
        break;
    };

    return pixelFormat;
}
}
