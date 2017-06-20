#include <map>

#include <Fever/FeverMetalWrapper.h>

namespace fv {
FvResult MetalWrapper::init(const FvInitInfo *initInfo) {
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

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::shutdown() { FV_MTL_RELEASE(device); }

FvResult
MetalWrapper::descriptorSetCreate(FvDescriptorSet *descriptorSet,
                                  const FvDescriptorSetCreateInfo *createInfo) {
    if (descriptorSet == nullptr || createInfo == nullptr) {
        return FV_RESULT_FAILURE;
    }

    DescriptorSetWrapper descriptorSetWrapper;

    // Loop thru the descriptors we've been asked to create and add them to the
    // descriptor set
    for (uint32_t i = 0; i < createInfo->descriptorCount; ++i) {
        FvDescriptorInfo descriptorInfo = createInfo->descriptors[i];

        switch (descriptorInfo.descriptorType) {
        case FV_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
            DescriptorBufferBinding bufferBinding;
            bufferBinding.descriptorInfo    = descriptorInfo;
            bufferBinding.bufferInfo.buffer = FV_NULL_HANDLE;
            bufferBinding.bufferInfo.offset = 0;
            bufferBinding.bufferInfo.range  = 0;

            descriptorSetWrapper.bufferBindings.push_back(bufferBinding);
            break;
        }
        case FV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
            DescriptorImageBinding imageBinding;
            imageBinding.descriptorInfo    = descriptorInfo;
            imageBinding.imageInfo.image   = FV_NULL_HANDLE;
            imageBinding.imageInfo.sampler = FV_NULL_HANDLE;

            descriptorSetWrapper.imageBindings.push_back(imageBinding);
            break;
        }
        default:
            break;
        }
    }

    // Store the descriptor set
    const Handle *handle = descriptorSets.add(descriptorSetWrapper);

    // Return the descriptor set
    *descriptorSet = (FvDescriptorSet)handle;

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::descriptorSetDestroy(FvDescriptorSet descriptorSet) {
    const Handle *handle = (const Handle *)descriptorSet;

    if (handle != nullptr) {
        descriptorSets.remove(*handle);
    }
}

// FvResult MetalWrapper::descriptorPoolCreate(
//     FvDescriptorPool *descriptorPool,
//     const FvDescriptorPoolCreateInfo *createInfo) {
//     FvResult result = FV_RESULT_FAILURE;

//     if (descriptorPool != nullptr && createInfo != nullptr) {
//         DescriptorPoolWrapper descriptorPoolWrapper;

//         descriptorPoolWrapper.maxSets = createInfo->maxSets;

//         for (uint32_t i = 0; i < createInfo->poolSizeCount; ++i) {
//             DescriptorPoolWrapper::Pool pool;

//             pool.descriptorSetsType =
//             createInfo->poolSizes[i].descriptorType;

//             descriptorPoolWrapper.pools.push_back(pool);
//         }

//         const Handle *handle = descriptorPools.add(descriptorPoolWrapper);

//         if (handle != nullptr) {
//             *descriptorPool = (FvDescriptorPool)handle;
//             result          = FV_RESULT_SUCCESS;
//         }
//     }

//     return result;
// }

// void MetalWrapper::descriptorPoolDestroy(FvDescriptorPool descriptorPool) {
//     const Handle *handle = (const Handle *)descriptorPool;

//     if (handle != nullptr) {
//         DescriptorPoolWrapper *descriptorPoolWrapper =
//             descriptorPools.get(*handle);

//         if (descriptorPoolWrapper != nullptr) {
//             // Free descriptor sets associated with each pool
//             for (uint32_t i = 0; i < descriptorPoolWrapper->pools.size();
//             ++i) {
//                 for (uint32_t j = 0;
//                      j <
//                      descriptorPoolWrapper->pools[i].descriptorSets.size();
//                      ++j) {
//                     // Remove descriptor set from internal store
//                     descriptorSets.remove(
//                         *((const Handle *)descriptorPoolWrapper->pools[i]
//                               .descriptorSets[j]));
//                 }
//             }
//         }

//         // Remove descriptor pool from internal store
//         descriptorPools.remove(*handle);
//     }
// }

// FvResult MetalWrapper::allocateDescriptorSets(
//     FvDescriptorSet *descriptorSets,
//     const FvDescriptorSetAllocateInfo *allocateInfo) {
//     FvResult result = FV_RESULT_FAILURE;

//     if (descriptorSets != nullptr && allocateInfo != nullptr) {
//         // Get descriptor pool to allocate from
//         const Handle *handle = (const Handle *)allocateInfo->descriptorPool;

//         if (handle != nullptr) {
//             DescriptorPoolWrapper *descriptorPoolWrapper =
//                 descriptorPools.get(*handle);

//             if (descriptorPoolWrapper != nullptr) {
//                 // Fill the descriptor set with set layouts
//                 for (uint32_t i = 0; i < allocateInfo->descriptorSetCount;
//                      ++i) {
//                     DescriptorSetWrapper descriptorSetWrapper;

//                     FvDescriptorSetLayout descriptorSetLayoutHandle =
//                         allocateInfo->setLayouts[i];
//                     descriptorSetWrapper.descriptorSetLayout =
//                         descriptorSetLayoutHandle;

//                     // Get descriptor set layout wrapper
//                     DescriptorSetLayoutWrapper *descriptorSetLayoutWrapper =
//                         this->descriptorSetLayouts.get(
//                             *((const Handle *)descriptorSetLayoutHandle));

//                     if (descriptorSetLayoutWrapper != nullptr) {
//                         // Loop thru each binding and ensure an appropriate
//                         pool
//                         // can be found to allocate the binding
//                         for (uint32_t j = 0;
//                              j < descriptorSetLayoutWrapper
//                                      ->descriptorSetLayoutBindings.size();
//                              ++j) {
//                             FvDescriptorType descriptorType =
//                                 descriptorSetLayoutWrapper
//                                     ->descriptorSetLayoutBindings[j]
//                                     .descriptorType;

//                             bool poolFound = false;
//                             for (uint32_t k = 0;
//                                  k < descriptorPoolWrapper->pools.size();
//                                  ++k) {
//                                 if (descriptorType ==
//                                     descriptorPoolWrapper->pools[k]
//                                         .descriptorSetsType) {
//                                     poolFound = true;
//                                     // Add descriptor set to internal store
//                                     const Handle *handle =
//                                         this->descriptorSets.add(
//                                             descriptorSetWrapper);

//                                     if (handle != nullptr) {
//                                         // Return handle
//                                         descriptorSets[i] =
//                                             (FvDescriptorSet)handle;

//                                         // Add handle to descriptor pool
//                                         descriptorPoolWrapper->pools[k]
//                                             .descriptorSets.push_back(
//                                                 (FvDescriptorSet)handle);

//                                         result = FV_RESULT_SUCCESS;
//                                     }

//                                     break;
//                                 }
//                             }

//                             if (!poolFound) {
//                                 return FV_RESULT_FAILURE;
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     return result;
// }

void MetalWrapper::updateDescriptorSets(
    uint32_t descriptorWriteCount,
    const FvWriteDescriptorSet *descriptorWrites) {
    // For each write
    for (uint32_t i = 0; i < descriptorWriteCount; ++i) {
        FvWriteDescriptorSet write = descriptorWrites[i];

        // Get descriptor set to write to
        DescriptorSetWrapper *descSet =
            descriptorSets.get(*((const Handle *)write.dstSet));

        // Find which descriptor to write to in descriptor set using binding
        // point as key
        switch (write.descriptorType) {
        case FV_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
            DescriptorBufferBinding *bufferBinding =
                descSet->getBufferBinding(write.dstBinding);

            if (bufferBinding != nullptr) {
                bufferBinding->bufferInfo = *(write.bufferInfo);
            }
            break;
        }
        case FV_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
            DescriptorImageBinding *imageBinding =
                descSet->getImageBinding(write.dstBinding);

            if (imageBinding != nullptr) {
                imageBinding->imageInfo = *(write.imageInfo);
            }
            break;
        }
        default:
            break;
        }
    }
}

// FvResult MetalWrapper::descriptorSetLayoutCreate(
//     FvDescriptorSetLayout *descriptorSetLayout,
//     const FvDescriptorSetLayoutCreateInfo *createInfo) {
//     FvResult result = FV_RESULT_FAILURE;

//     if (descriptorSetLayout != nullptr && createInfo != nullptr) {
//         DescriptorSetLayoutWrapper descriptorSetLayoutWrapper;

//         for (uint32_t i = 0; i < createInfo->bindingCount; ++i) {
//             descriptorSetLayoutWrapper.descriptorSetLayoutBindings.push_back(
//                 createInfo->bindings[i]);
//         }

//         const Handle *handle =
//             descriptorSetLayouts.add(descriptorSetLayoutWrapper);

//         if (handle != nullptr) {
//             *descriptorSetLayout = (FvDescriptorSetLayout)handle;
//             result               = FV_RESULT_SUCCESS;
//         }
//     }

//     return result;
// }

// void MetalWrapper::descriptorSetLayoutDestroy(
//     FvDescriptorSetLayout descriptorSetLayout) {
//     const Handle *handle = (const Handle *)descriptorSetLayout;

//     if (handle != nullptr) {
//         descriptorSetLayouts.remove(*handle);
//     }
// }

FvResult MetalWrapper::bufferCreate(FvBuffer *buffer,
                                    const FvBufferCreateInfo *createInfo) {
    FvResult result = FV_RESULT_FAILURE;

    if (buffer != nullptr && createInfo != nullptr) {
        id<MTLBuffer> mtlBuffer = nil;

        if (createInfo->data == nullptr) {
            mtlBuffer = [device newBufferWithLength:createInfo->size options:0];
        } else {
            mtlBuffer = [device newBufferWithBytes:createInfo->data
                                            length:createInfo->size
                                           options:0];
        }

        BufferWrapper bufferWrapper;
        bufferWrapper.mtlBuffer = mtlBuffer;

        const Handle *handle = buffers.add(bufferWrapper);

        if (handle != nullptr) {
            *buffer = (FvBuffer)handle;
            result  = FV_RESULT_SUCCESS;
        }
    }

    return result;
}

void MetalWrapper::bufferDestroy(FvBuffer buffer) {
    const Handle *handle = (const Handle *)buffer;

    if (handle != nullptr) {
        BufferWrapper *bufferWrapper = buffers.get(*handle);

        if (bufferWrapper != nullptr) {
            FV_MTL_RELEASE(bufferWrapper->mtlBuffer);
        }

        buffers.remove(*handle);
    }
}

void MetalWrapper::bufferReplaceData(FvBuffer buffer, void *data,
                                     size_t dataSize) {
    // Get buffer wrapper
    BufferWrapper *bufferWrapper = buffers.get(*((const Handle *)buffer));

    if (bufferWrapper != nullptr) {
        // Get buffer data pointer
        void *bufferData = [bufferWrapper->mtlBuffer contents];
        // Copy data to buffer
        memcpy(bufferData, data, dataSize);
        // Notify GPU that we modified buffer's contents (only applies if
        // storage mode is managed)
        // [bufferWrapper->mtlBuffer
        //     didModifyRange:NSMakeRange((NSUInteger)bufferData, dataSize)];
    }
}

FvResult MetalWrapper::semaphoreCreate(FvSemaphore *semaphore) {
    FvResult result = FV_RESULT_FAILURE;

    if (semaphore != nullptr) {
        SemaphoreWrapper semaphoreWrapper;

        const Handle *handle = semaphores.add(semaphoreWrapper);

        if (handle != nullptr) {
            *semaphore = (FvSemaphore)handle;
            result     = FV_RESULT_SUCCESS;
        }
    }

    return result;
}

void MetalWrapper::semaphoreDestroy(FvSemaphore semaphore) {
    const Handle *handle = (const Handle *)semaphore;

    if (handle != nullptr) {
        SemaphoreWrapper *semaphoreWrapper = semaphores.get(*handle);

        if (semaphoreWrapper != nullptr) {
            semaphoreWrapper->release();
        }

        semaphores.remove(*handle);
    }
}

FvResult MetalWrapper::acquireNextImage(FvSwapchain swapchain,
                                        FvSemaphore imageAvailableSemaphore) {
    const Handle *handle = (const Handle *)swapchain;

    if (handle == nullptr) {
        return FV_RESULT_FAILURE;
    }

    SwapchainWrapper *swapchainWrapper = swapchains.get(*handle);

    CGSize drawableSize;
    drawableSize.width  = swapchainWrapper->extent.width;
    drawableSize.height = swapchainWrapper->extent.height;

    metalLayer.drawableSize = drawableSize;

    currentDrawable = [metalLayer nextDrawable];

    // Signal semaphore immediately
    // (Metal nextDrawable is blocking and so when it returns, image is ready)
    handle = (const Handle *)imageAvailableSemaphore;

    if (handle != nullptr) {
        SemaphoreWrapper *semaphoreWrapper = semaphores.get(*handle);

        if (semaphoreWrapper != nullptr) {
            semaphoreWrapper->signal();
        }
    }

    return FV_RESULT_SUCCESS;
}

FvResult
MetalWrapper::createSwapchain(FvSwapchain *swapchain,
                              const FvSwapchainCreateInfo *createInfo) {
    if (createInfo == nullptr || swapchain == nullptr) {
        return FV_RESULT_FAILURE;
    }

    SwapchainWrapper swapchainWrapper;
    swapchainWrapper.extent = createInfo->extent;

    // Store swapchain wrapper and return handle
    const Handle *handle = swapchains.add(swapchainWrapper);

    if (handle != nullptr) {
        *swapchain = (FvSwapchain)handle;
    } else {
        return FV_RESULT_FAILURE;
    }

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::destroySwapchain(FvSwapchain swapchain) {
    const Handle *handle = (const Handle *)swapchain;

    if (handle != nullptr) {
        // SwapchainWrapper *swapchainWrapper = swapchains.get(*handle);

        swapchains.remove(*handle);
    }
}

void MetalWrapper::getSwapchainImage(FvSwapchain swapchain,
                                     FvImage *swapchainImage) {
    // Create drawable image
    ImageWrapper imageWrapper;
    imageWrapper.isDrawable = true;
    imageWrapper.texture    = nil;

    // Store texture and return handle
    const Handle *handle = textures.add(imageWrapper);

    if (handle != nullptr) {
        *swapchainImage = (FvImage)handle;
    }
}

void MetalWrapper::queuePresent(const FvPresentInfo *presentInfo) {
    if (presentInfo != nullptr) {
        // Wait for semaphores
        for (uint32_t i = 0; i < presentInfo->waitSemaphoreCount; ++i) {
            FvSemaphore semaphore = presentInfo->waitSemaphores[i];

            // Get internal semaphore
            const Handle *handle = (const Handle *)semaphore;

            if (handle != nullptr) {
                SemaphoreWrapper *semaphoreWrapper = semaphores.get(*handle);

                if (semaphoreWrapper != nullptr) {
                    semaphoreWrapper->wait();
                }
            }
        }

        // TODO: Expand for multiple presentation images
        // uint32_t imageIndex = presentInfo->imageIndices[0];

        @autoreleasepool {
            // Make separate command buffer to schedule drawable presentation
            // and submit it
            id<MTLCommandBuffer> commandBuffer =
                [currentCommandQueue commandBuffer];

            [commandBuffer presentDrawable:currentDrawable];

            [commandBuffer commit];

            currentDrawable = nil;

            currentCommandQueue = nil;
        }

        // Can now re-use drawable, so free it from swapchain
        /*
        for (uint32_t i = 0; i < currentSwapchain.size(); ++i) {
            if (i == imageIndex) {
                currentSwapchain[i] = nil;
            }
        }
        */
    }
}

FvResult MetalWrapper::queueSubmit(uint32_t submissionsCount,
                                   const FvSubmitInfo *submissions) {
    if (submissions == nullptr && submissionsCount != 0) {
        return FV_RESULT_FAILURE;
    }

    // Loop thru each submission
    for (uint32_t i = 0; i < submissionsCount; ++i) {

        // Wait for semaphores
        for (uint32_t j = 0; j < submissions[i].waitSemaphoreCount; ++j) {
            FvSemaphore semaphore = submissions[i].waitSemaphores[j];

            // Get internal semaphore
            const Handle *handle = (const Handle *)semaphore;

            if (handle != nullptr) {
                SemaphoreWrapper *semaphoreWrapper = semaphores.get(*handle);

                if (semaphoreWrapper != nullptr) {
                    semaphoreWrapper->wait();
                }
            }
        }

        // Synchronization code from this excellent answer on SO:
        // http://stackoverflow.com/a/20910658/
        // Create a group of tasks (each command buffers work is a task)
        // A group is created so we can be notified when they are ALL finished.
        dispatch_group_t group = dispatch_group_create();

        // Submit each command buffer
        for (uint32_t j = 0; j < submissions[i].commandBufferCount; ++j) {
            // Enter the group (essentially adding a new task that will wait for
            // completion)
            dispatch_group_enter(group);

            FvCommandBuffer commandBufferHandle =
                submissions[i].commandBuffers[j];
            const Handle *handle = (const Handle *)commandBufferHandle;

            // Get command buffer from handle
            CommandBufferWrapper *commandBufferWrapper = nullptr;

            if (handle == nullptr) {
                return FV_RESULT_FAILURE;
            }

            commandBufferWrapper = commandBuffers.get(*handle);

            if (commandBufferWrapper == nullptr) {
                return FV_RESULT_FAILURE;
            }

            // For each draw item, sort by render pass
            std::map<MTLRenderPassDescriptor *, std::vector<DrawItem *>>
                drawItemsByRenderPass;
            for (uint32_t iDrawItem = 0;
                 iDrawItem < commandBufferWrapper->drawItems.size();
                 ++iDrawItem) {
                DrawItem *drawItem =
                    &commandBufferWrapper->drawItems[iDrawItem];

                if (drawItem->finished != true) {
                    return FV_RESULT_FAILURE;
                }

                // Get graphics pipeline wrapper
                handle = (const Handle *)drawItem->graphicsPipeline;

                if (handle == nullptr) {
                    return FV_RESULT_FAILURE;
                }

                GraphicsPipelineWrapper *pipeline = nullptr;

                pipeline = graphicsPipelines.get(*handle);

                if (pipeline == nullptr) {
                    return FV_RESULT_FAILURE;
                }

                // Loop through each render pass color attachment, check if it's
                // texture is a drawable. If it is, back it with the current
                // drawable.
                for (uint32_t k = 0; k < pipeline->colorAttachments.size();
                     ++k) {
                    if (commandBufferWrapper->attachments[k].isDrawable ==
                        true) {
                        pipeline->renderPass.colorAttachments[k].texture =
                            currentDrawable.texture;
                    }
                }

                drawItemsByRenderPass[pipeline->renderPass].push_back(drawItem);
            }

            @autoreleasepool {
                // Create command buffer from it's command queue Command buffers
                // are transient, single-use objects in Metal and so are created
                // here at the last minute.
                id<MTLCommandBuffer> commandBuffer =
                    [commandBufferWrapper->commandQueue commandBuffer];

                // Set current command queue
                currentCommandQueue = commandBufferWrapper->commandQueue;

                // Encode draw items
                for (auto const &renderPassDrawItems : drawItemsByRenderPass) {
                    // Create command encoder from command buffer and render
                    // pass descriptor
                    id<MTLRenderCommandEncoder> encoder = [commandBuffer
                        renderCommandEncoderWithDescriptor:renderPassDrawItems
                                                               .first];

                    for (uint32_t iDrawItem = 0;
                         iDrawItem < renderPassDrawItems.second.size();
                         ++iDrawItem) {
                        DrawItem *drawItem =
                            renderPassDrawItems.second[iDrawItem];

                        // Get graphics pipeline wrapper
                        handle = (const Handle *)drawItem->graphicsPipeline;

                        if (handle == nullptr) {
                            return FV_RESULT_FAILURE;
                        }

                        GraphicsPipelineWrapper *pipeline = nullptr;

                        pipeline = graphicsPipelines.get(*handle);

                        if (pipeline == nullptr) {
                            return FV_RESULT_FAILURE;
                        }

                        // Special case, no vertex buffer, can still encode
                        // commands
                        if (drawItem->vertexBuffers.size() == 0) {
                            // Set states
                            [encoder setCullMode:pipeline->cullMode];
                            [encoder setDepthStencilState:pipeline->
                                                          depthStencilState];
                            [encoder
                                setFrontFacingWinding:pipeline->windingOrder];
                            [encoder
                                setRenderPipelineState:pipeline->
                                                       renderPipelineState];
                            [encoder setScissorRect:pipeline->scissor];
                            [encoder setViewport:pipeline->viewport];

                            // Bind descriptor sets
                            for (uint32_t iDescSet = 0;
                                 iDescSet < drawItem->descriptorSets.size();
                                 ++iDescSet) {
                                // Get descriptor set
                                FvDescriptorSet descSet =
                                    drawItem->descriptorSets[iDescSet];
                                const DescriptorSetWrapper *descSetWrapper =
                                    descriptorSets.get(
                                        *((const Handle *)descSet));

                                if (descSetWrapper != nullptr) {
                                    // Bind buffers
                                    for (uint32_t iBufferBinding = 0;
                                         iBufferBinding <
                                         descSetWrapper->bufferBindings.size();
                                         ++iBufferBinding) {
                                        DescriptorBufferBinding bufferBinding =
                                            descSetWrapper->bufferBindings
                                                [iBufferBinding];

                                        // Get buffer to bind
                                        BufferWrapper *bufferWrapper =
                                            buffers.get(
                                                *((const Handle *)bufferBinding
                                                      .bufferInfo.buffer));

                                        if (bufferWrapper != nullptr) {
                                            int stageFlags =
                                                bufferBinding.descriptorInfo
                                                    .stageFlags;
                                            id<MTLBuffer> mtlBufferToBind =
                                                bufferWrapper->mtlBuffer;
                                            FvSize offset =
                                                bufferBinding.bufferInfo.offset;
                                            uint32_t bindingPoint =
                                                bufferBinding.descriptorInfo
                                                    .binding;

                                            if (stageFlags &
                                                FV_SHADER_STAGE_VERTEX) {
                                                [encoder
                                                    setVertexBuffer:
                                                        mtlBufferToBind
                                                             offset:offset
                                                            atIndex:
                                                                bindingPoint];
                                            }
                                            if (stageFlags &
                                                FV_SHADER_STAGE_FRAGMENT) {
                                                [encoder
                                                    setFragmentBuffer:
                                                        mtlBufferToBind
                                                               offset:offset
                                                              atIndex:
                                                                  bindingPoint];
                                            }
                                        }
                                    }
                                    // Bind images
                                    for (uint32_t iImageBinding = 0;
                                         iImageBinding <
                                         descSetWrapper->imageBindings.size();
                                         ++iImageBinding) {
                                        DescriptorImageBinding imageBinding =
                                            descSetWrapper
                                                ->imageBindings[iImageBinding];

                                        // Get image to bind
                                        ImageWrapper *imageWrapper =
                                            textures.get(
                                                *((const Handle *)imageBinding
                                                      .imageInfo.image));

                                        if (imageWrapper != nullptr) {
                                            int stageFlags =
                                                imageBinding.descriptorInfo
                                                    .stageFlags;
                                            id<MTLTexture> mtlImageToBind =
                                                imageWrapper->texture;
                                            uint32_t bindingPoint =
                                                imageBinding.descriptorInfo
                                                    .binding;
                                            id<MTLSamplerState>
                                                *mtlSamplerState = samplers.get(
                                                    *((const Handle *)
                                                          imageBinding.imageInfo
                                                              .sampler));

                                            if (stageFlags &
                                                FV_SHADER_STAGE_VERTEX) {
                                                [encoder
                                                    setVertexTexture:
                                                        mtlImageToBind
                                                             atIndex:
                                                                 bindingPoint];
                                                [encoder
                                                    setVertexSamplerState:
                                                        *mtlSamplerState
                                                                  atIndex:
                                                                      bindingPoint];
                                            }
                                            if (stageFlags &
                                                FV_SHADER_STAGE_FRAGMENT) {
                                                [encoder
                                                    setFragmentTexture:
                                                        mtlImageToBind
                                                               atIndex:
                                                                   bindingPoint];
                                                [encoder
                                                    setFragmentSamplerState:
                                                        *mtlSamplerState
                                                                    atIndex:
                                                                        bindingPoint];
                                            }
                                        }
                                    }
                                }
                            }

                            // Make draw call
                            DrawCallNonIndexed dc =
                                drawItem->drawCall.nonIndexed;
                            [encoder drawPrimitives:pipeline->primitiveType
                                        vertexStart:dc.firstVertex
                                        vertexCount:dc.vertexCount
                                      instanceCount:dc.instanceCount
                                       baseInstance:dc.firstInstance];
                            // End encoding
                            [encoder endEncoding];
                        } else {
                            // otherwise, for each vertex buffer
                            for (size_t iVertexBuffer = 0;
                                 iVertexBuffer < drawItem->vertexBuffers.size();
                                 ++iVertexBuffer) {
                                // Get vertex buffer
                                const Handle *vertexBufferHandle =
                                    (const Handle *)
                                        drawItem->vertexBuffers[iVertexBuffer];

                                const BufferWrapper *vertexBufferWrapper =
                                    nullptr;
                                vertexBufferWrapper =
                                    buffers.get(*vertexBufferHandle);

                                if (vertexBufferWrapper != nullptr) {
                                    // Set states
                                    [encoder setCullMode:pipeline->cullMode];
                                    [encoder
                                        setDepthStencilState:pipeline->
                                                             depthStencilState];
                                    [encoder
                                        setFrontFacingWinding:pipeline->
                                                              windingOrder];
                                    [encoder setRenderPipelineState:
                                                 pipeline->renderPipelineState];
                                    [encoder setScissorRect:pipeline->scissor];
                                    [encoder setViewport:pipeline->viewport];
                                    [encoder
                                        setVertexBuffer:vertexBufferWrapper
                                                            ->mtlBuffer
                                                 offset:vertexBufferWrapper
                                                            ->offset
                                                atIndex:vertexBufferWrapper->
                                                        bindingPoint];

                                    // Bind descriptor sets (uniform buffers)
                                    for (uint32_t iDescSet = 0;
                                         iDescSet <
                                         drawItem->descriptorSets.size();
                                         ++iDescSet) {
                                        // Get descriptor set
                                        FvDescriptorSet descSet =
                                            drawItem->descriptorSets[iDescSet];
                                        const DescriptorSetWrapper *
                                            descSetWrapper = descriptorSets.get(
                                                *((const Handle *)descSet));

                                        if (descSetWrapper != nullptr) {
                                            // Bind buffers
                                            for (uint32_t iBufferBinding = 0;
                                                 iBufferBinding <
                                                 descSetWrapper->bufferBindings
                                                     .size();
                                                 ++iBufferBinding) {
                                                DescriptorBufferBinding
                                                    bufferBinding =
                                                        descSetWrapper
                                                            ->bufferBindings
                                                                [iBufferBinding];

                                                // Get buffer to bind
                                                BufferWrapper *bufferWrapper =
                                                    buffers.get(
                                                        *((const Handle *)
                                                              bufferBinding
                                                                  .bufferInfo
                                                                  .buffer));

                                                if (bufferWrapper != nullptr) {
                                                    int stageFlags =
                                                        bufferBinding
                                                            .descriptorInfo
                                                            .stageFlags;
                                                    id<MTLBuffer>
                                                        mtlBufferToBind =
                                                            bufferWrapper
                                                                ->mtlBuffer;
                                                    FvSize offset =
                                                        bufferBinding.bufferInfo
                                                            .offset;
                                                    uint32_t bindingPoint =
                                                        bufferBinding
                                                            .descriptorInfo
                                                            .binding;

                                                    if (stageFlags &
                                                        FV_SHADER_STAGE_VERTEX) {
                                                        [encoder
                                                            setVertexBuffer:
                                                                mtlBufferToBind
                                                                     offset:
                                                                         offset
                                                                    atIndex:
                                                                        bindingPoint];
                                                    }
                                                    if (stageFlags &
                                                        FV_SHADER_STAGE_FRAGMENT) {
                                                        [encoder
                                                            setFragmentBuffer:
                                                                mtlBufferToBind
                                                                       offset:
                                                                           offset
                                                                      atIndex:
                                                                          bindingPoint];
                                                    }
                                                }
                                            }
                                            // Bind images
                                            for (uint32_t iImageBinding = 0;
                                                 iImageBinding <
                                                 descSetWrapper->imageBindings
                                                     .size();
                                                 ++iImageBinding) {
                                                DescriptorImageBinding
                                                    imageBinding =
                                                        descSetWrapper
                                                            ->imageBindings
                                                                [iImageBinding];

                                                // Get image to bind
                                                ImageWrapper *imageWrapper =
                                                    textures.get(*(
                                                        (const Handle
                                                             *)imageBinding
                                                            .imageInfo.image));

                                                if (imageWrapper != nullptr) {
                                                    int stageFlags =
                                                        imageBinding
                                                            .descriptorInfo
                                                            .stageFlags;
                                                    id<MTLTexture>
                                                        mtlImageToBind =
                                                            imageWrapper
                                                                ->texture;
                                                    uint32_t bindingPoint =
                                                        imageBinding
                                                            .descriptorInfo
                                                            .binding;
                                                    id<MTLSamplerState>
                                                        *mtlSamplerState =
                                                            samplers.get(*(
                                                                (const Handle
                                                                     *)imageBinding
                                                                    .imageInfo
                                                                    .sampler));

                                                    if (stageFlags &
                                                        FV_SHADER_STAGE_VERTEX) {
                                                        [encoder
                                                            setVertexTexture:
                                                                mtlImageToBind
                                                                     atIndex:
                                                                         bindingPoint];
                                                        [encoder
                                                            setVertexSamplerState:
                                                                *mtlSamplerState
                                                                          atIndex:
                                                                              bindingPoint];
                                                    }
                                                    if (stageFlags &
                                                        FV_SHADER_STAGE_FRAGMENT) {
                                                        [encoder
                                                            setFragmentTexture:
                                                                mtlImageToBind
                                                                       atIndex:
                                                                           bindingPoint];
                                                        [encoder
                                                            setFragmentSamplerState:
                                                                *mtlSamplerState
                                                                            atIndex:
                                                                                bindingPoint];
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    if (drawItem->drawCall.type ==
                                        DRAW_CALL_TYPE_INDEXED) {
                                        // Get index buffer from command buffer
                                        // wrapper
                                        const BufferWrapper *indexBuf =
                                            buffers.get(
                                                *((const Handle *)
                                                      drawItem->indexBuffer));

                                        if (indexBuf != nullptr) {
                                            // Make indexed draw call
                                            DrawCallIndexed dc =
                                                drawItem->drawCall.indexed;
                                            [encoder
                                                drawIndexedPrimitives:
                                                    pipeline->primitiveType
                                                           indexCount:
                                                               dc.indexCount
                                                            indexType:
                                                                indexBuf
                                                                    ->mtlIndexType
                                                          indexBuffer:
                                                              indexBuf
                                                                  ->mtlBuffer
                                                    indexBufferOffset:
                                                        indexBuf->offset
                                                        instanceCount:
                                                            dc.instanceCount
                                                           baseVertex:
                                                               dc.vertexOffset
                                                         baseInstance:
                                                             dc.firstInstance];
                                        }
                                    } else {
                                        // Make non-indexed draw call
                                        DrawCallNonIndexed dc =
                                            drawItem->drawCall.nonIndexed;
                                        [encoder
                                            drawPrimitives:pipeline
                                                               ->primitiveType
                                               vertexStart:dc.firstVertex
                                               vertexCount:dc.vertexCount
                                             instanceCount:dc.instanceCount
                                              baseInstance:dc.firstInstance];
                                    }
                                }
                            }
                        }
                    }

                    // End encoding
                    [encoder endEncoding];
                }

                [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
                  // Indicate that this task has finished
                  dispatch_group_leave(group);
                }];

                // Commit command buffer
                [commandBuffer commit];
            }
        }

        // A copy of the submission object is made here so that we use it,
        // rather than the submissions pointer to access the submission object
        // in the block below. If we use the pointer, it may no longer point to
        // a valid object (the submission info) when the block begins.
        //
        // We can access and mutate the 'semaphores' member variable within
        // block because blocks make a strong, mutable referenc to member
        // variables used inside blocks. Blocks and Objects reference:
        // https://developer.apple.com/library/content/documentation/Cocoa/Conceptual/Blocks/Articles/bxVariables.html#//apple_ref/doc/uid/TP40007502-CH6-SW4

        FvSubmitInfo submission = submissions[i];

        // Notify us when all tasks are done
        dispatch_group_notify(
            group, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0),
            ^{
              // Signal semaphores on completion
              for (uint32_t i = 0; i < submission.signalSemaphoreCount; ++i) {
                  FvSemaphore semaphore = submission.signalSemaphores[i];

                  // Get internal semaphore
                  const Handle *handle = (const Handle *)semaphore;

                  if (handle != nullptr) {
                      SemaphoreWrapper *semaphoreWrapper =
                          semaphores.get(*handle);

                      if (semaphoreWrapper != nullptr) {
                          semaphoreWrapper->signal();
                      }
                  }
              }
            });

        // Make sure to release group. We can release here because: "The system
        // holds a reference to the dispatch group while an asynchronous
        // notification is pending, therefore it is valid to release the group
        // after setting a notification block" from:
        // https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/dispatch_group_notify.3.html
        dispatch_release(group);
    }

    return FV_RESULT_SUCCESS;
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

    // Find first draw item not yet finished or create one if no draw items not
    // finished or no draw items at all
    DrawItem *drawItem = nullptr;
    for (size_t i = 0; i < commandBufferWrapper->drawItems.size(); ++i) {
        if (commandBufferWrapper->drawItems[i].finished == false) {
            drawItem = &commandBufferWrapper->drawItems[i];
        }
    }

    if (drawItem == nullptr) {
        commandBufferWrapper->drawItems.push_back(DrawItem());
        drawItem =
            &(commandBufferWrapper
                  ->drawItems[commandBufferWrapper->drawItems.size() - 1]);
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
            commandBufferWrapper->attachments[attachmentIndex].texture;

        pipelineWrapper->renderPass.colorAttachments[i].clearColor =
            MTLClearColorMake(clearColor[0], clearColor[1], clearColor[2],
                              clearColor[3]);
    }

    // Should only be one depth stencil attachment per framebuffer
    if (pipelineWrapper->depthAttachment.size() == 1) {
        uint32_t attachmentIndex =
            pipelineWrapper->depthAttachment[0].attachment;

        pipelineWrapper->renderPass.depthAttachment.texture =
            commandBufferWrapper->attachments[attachmentIndex].texture;

        pipelineWrapper->renderPass.depthAttachment.clearDepth =
            commandBufferWrapper->clearValues[attachmentIndex]
                .depthStencil.depth;
    }
    if (pipelineWrapper->stencilAttachment.size() == 1) {
        uint32_t attachmentIndex =
            pipelineWrapper->stencilAttachment[0].attachment;

        pipelineWrapper->renderPass.stencilAttachment.texture =
            commandBufferWrapper->attachments[attachmentIndex].texture;

        pipelineWrapper->renderPass.stencilAttachment.clearStencil =
            commandBufferWrapper->clearValues[attachmentIndex]
                .depthStencil.stencil;
    }

    // Associate graphics pipeline with command buffer
    drawItem->graphicsPipeline = graphicsPipeline;
}

void MetalWrapper::cmdBindVertexBuffers(FvCommandBuffer commandBuffer,
                                        uint32_t firstBinding,
                                        uint32_t bindingCount,
                                        const FvBuffer *buffers,
                                        const FvSize *offsets) {
    // Get command buffer
    CommandBufferWrapper *commandBufferWrapper = nullptr;

    const Handle *commandBufferHandle = (const Handle *)commandBuffer;

    if (commandBufferHandle != nullptr) {
        commandBufferWrapper = commandBuffers.get(*commandBufferHandle);
    }

    if (commandBufferWrapper == nullptr || buffers == nullptr ||
        offsets == nullptr) {
        return;
    }

    // Find first draw item not yet finished or create one if no draw items not
    // finished or no draw items at all
    DrawItem *drawItem = nullptr;
    for (size_t i = 0; i < commandBufferWrapper->drawItems.size(); ++i) {
        if (commandBufferWrapper->drawItems[i].finished == false) {
            drawItem = &commandBufferWrapper->drawItems[i];
        }
    }

    if (drawItem == nullptr) {
        commandBufferWrapper->drawItems.push_back(DrawItem());
        drawItem =
            &(commandBufferWrapper
                  ->drawItems[commandBufferWrapper->drawItems.size() - 1]);
    }

    // Loop thru each buffer to bind
    for (uint32_t i = 0; i < bindingCount; ++i) {
        // Get binding point and offset of buffer
        FvSize bindingPoint = firstBinding + i;
        FvSize offset       = offsets[i];

        // Get buffer wrapper
        BufferWrapper *bufferWrapper = nullptr;
        const Handle *bufferHandle   = (const Handle *)buffers[i];
        if (bufferHandle != nullptr) {
            bufferWrapper = this->buffers.get(*bufferHandle);
        }

        assert(bufferWrapper != nullptr);
        if (bufferWrapper != nullptr) {
            // Fill out binding point and offset information
            bufferWrapper->bindingPoint = bindingPoint;
            bufferWrapper->offset       = offset;

            // Attach vertex buffer to command buffer
            drawItem->vertexBuffers.push_back((FvBuffer)bufferHandle);
        }
    }
}

void MetalWrapper::cmdBindIndexBuffer(FvCommandBuffer commandBuffer,
                                      FvBuffer buffer, FvSize offset,
                                      FvIndexType indexType) {
    // Get command buffer
    CommandBufferWrapper *commandBufferWrapper =
        commandBuffers.get(*((const Handle *)commandBuffer));

    // Get index buffer wrapper
    BufferWrapper *indexBufferWrapper = buffers.get(*((const Handle *)buffer));

    if (commandBufferWrapper == nullptr || indexBufferWrapper == nullptr) {
        return;
    }

    // Find first draw item not yet finished or create one if no draw items not
    // finished or no draw items at all
    DrawItem *drawItem = nullptr;
    for (size_t i = 0; i < commandBufferWrapper->drawItems.size(); ++i) {
        if (commandBufferWrapper->drawItems[i].finished == false) {
            drawItem = &commandBufferWrapper->drawItems[i];
        }
    }

    if (drawItem == nullptr) {
        commandBufferWrapper->drawItems.push_back(DrawItem());
        drawItem =
            &(commandBufferWrapper
                  ->drawItems[commandBufferWrapper->drawItems.size() - 1]);
    }

    // Setup index buffer
    indexBufferWrapper->mtlIndexType = toMtlIndexType(indexType);
    indexBufferWrapper->offset       = offset;

    // Bind index buffer to command buffer
    drawItem->indexBuffer = buffer;
}

void MetalWrapper::cmdBindDescriptorSets(
    FvCommandBuffer commandBuffer, FvPipelineLayout layout, uint32_t firstSet,
    uint32_t descriptorSetCount, const FvDescriptorSet *descriptorSets) {
    // Get command buffer
    CommandBufferWrapper *commandBufferWrapper =
        commandBuffers.get(*((const Handle *)commandBuffer));

    if (commandBufferWrapper == nullptr) {
        return;
    }

    // Find first draw item not yet finished or create one if no draw items not
    // finished or no draw items at all
    DrawItem *drawItem = nullptr;
    for (size_t i = 0; i < commandBufferWrapper->drawItems.size(); ++i) {
        if (commandBufferWrapper->drawItems[i].finished == false) {
            drawItem = &commandBufferWrapper->drawItems[i];
        }
    }

    if (drawItem == nullptr) {
        commandBufferWrapper->drawItems.push_back(DrawItem());
        drawItem =
            &(commandBufferWrapper
                  ->drawItems[commandBufferWrapper->drawItems.size() - 1]);
    }

    // Add each descriptor set to command buffer
    for (uint32_t i = 0; i < descriptorSetCount; ++i) {
        drawItem->descriptorSets.push_back(descriptorSets[i]);
    }
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

    // Find first draw item not yet finished or create one if no draw items not
    // finished or no draw items at all
    DrawItem *drawItem = nullptr;
    for (size_t i = 0; i < commandBufferWrapper->drawItems.size(); ++i) {
        if (commandBufferWrapper->drawItems[i].finished == false) {
            drawItem = &commandBufferWrapper->drawItems[i];
        }
    }

    if (drawItem == nullptr) {
        commandBufferWrapper->drawItems.push_back(DrawItem());
        drawItem =
            &(commandBufferWrapper
                  ->drawItems[commandBufferWrapper->drawItems.size() - 1]);
    }

    // Draw call made, draw item is finished
    drawItem->finished = true;

    if (commandBufferWrapper != nullptr) {
        drawItem->drawCall.type                   = DRAW_CALL_TYPE_NON_INDEXED;
        drawItem->drawCall.nonIndexed.vertexCount = vertexCount;
        drawItem->drawCall.nonIndexed.instanceCount = instanceCount;
        drawItem->drawCall.nonIndexed.firstVertex   = firstVertex;
        drawItem->drawCall.nonIndexed.firstInstance = firstInstance;
    }
}

void MetalWrapper::cmdDrawIndexed(FvCommandBuffer commandBuffer,
                                  uint32_t indexCount, uint32_t instanceCount,
                                  uint32_t firstIndex, int32_t vertexOffset,
                                  uint32_t firstInstance) {

    // Get command buffer
    CommandBufferWrapper *commandBufferWrapper =
        commandBuffers.get(*((const Handle *)commandBuffer));

    // Find first draw item not yet finished or create one if no draw items not
    // finished or no draw items at all
    DrawItem *drawItem = nullptr;
    for (size_t i = 0; i < commandBufferWrapper->drawItems.size(); ++i) {
        if (commandBufferWrapper->drawItems[i].finished == false) {
            drawItem = &commandBufferWrapper->drawItems[i];
        }
    }

    if (drawItem == nullptr) {
        commandBufferWrapper->drawItems.push_back(DrawItem());
        drawItem =
            &(commandBufferWrapper
                  ->drawItems[commandBufferWrapper->drawItems.size() - 1]);
    }

    // Draw call made, draw item is finished
    drawItem->finished = true;

    if (commandBufferWrapper != nullptr) {
        drawItem->drawCall.type                  = DRAW_CALL_TYPE_INDEXED;
        drawItem->drawCall.indexed.indexCount    = indexCount;
        drawItem->drawCall.indexed.instanceCount = instanceCount;
        drawItem->drawCall.indexed.firstIndex    = firstIndex;
        drawItem->drawCall.indexed.vertexOffset  = vertexOffset;
        drawItem->drawCall.indexed.firstInstance = firstInstance;
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
            FV_MTL_RELEASE(*commandQueue);
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
            ImageWrapper *texture = textures.get(*handle);

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
                                     mipmapped:createInfo->mipLevels > 1 ? YES
                                                                         : NO];

    } else {
        textureDesc = [MTLTextureDescriptor
            textureCubeDescriptorWithPixelFormat:toMtlPixelFormat(
                                                     createInfo->format)
                                            size:createInfo->extent.width
                                       mipmapped:createInfo->mipLevels > 1
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
    textureDesc.mipmapLevelCount = createInfo->mipLevels;
    textureDesc.sampleCount      = toMtlSampleCount(createInfo->samples);
    textureDesc.arrayLength      = createInfo->arrayLayers;
    textureDesc.usage            = toMtlTextureUsage(createInfo->usage);

    // Depth, Stencil, DepthStencil and Multisample textures must be allocated
    // with the MTLResourceStorageModePrivate resource option.
    if (textureDesc.pixelFormat == MTLPixelFormatDepth16Unorm ||
        textureDesc.pixelFormat == MTLPixelFormatDepth32Float ||
        textureDesc.pixelFormat == MTLPixelFormatDepth24Unorm_Stencil8 ||
        textureDesc.pixelFormat == MTLPixelFormatDepth32Float_Stencil8 ||
        textureDesc.sampleCount > 1) {
        textureDesc.storageMode = MTLStorageModePrivate;
    }

    // Create texture
    id<MTLTexture> texture = [device newTextureWithDescriptor:textureDesc];

    if (texture == nil) {
        return FV_RESULT_FAILURE;
    }

    ImageWrapper imageWrapper;
    imageWrapper.texture    = texture;
    imageWrapper.isDrawable = false;

    // Store texture and return handle
    const Handle *handle = textures.add(imageWrapper);

    if (handle != nullptr) {
        *image = (FvImage)handle;
    } else {
        return FV_RESULT_FAILURE;
    }

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::imageReplaceRegion(FvImage image, FvRect3D region,
                                      uint32_t mipLevel, uint32_t layer,
                                      void *data, size_t bytesPerRow,
                                      size_t bytesPerImage) {
    // Get metal image object
    const Handle *handle = (const Handle *)image;

    if (handle != nullptr) {
        ImageWrapper *imageWrapper = textures.get(*handle);

        if (imageWrapper != nullptr) {
            // Replace region
            MTLRegion mtlRegion;
            mtlRegion.origin.x    = region.origin.x;
            mtlRegion.origin.y    = region.origin.y;
            mtlRegion.origin.z    = region.origin.z;
            mtlRegion.size.width  = region.extent.width;
            mtlRegion.size.height = region.extent.height;
            mtlRegion.size.depth  = region.extent.depth;

            [imageWrapper->texture replaceRegion:mtlRegion
                                     mipmapLevel:mipLevel
                                           slice:layer
                                       withBytes:data
                                     bytesPerRow:bytesPerRow
                                   bytesPerImage:bytesPerImage];
        }
    }
}

void MetalWrapper::imageDestroy(FvImage image) {
    const Handle *handle = (const Handle *)image;

    if (handle != nullptr) {
        ImageWrapper *imageWrapper = textures.get(*handle);

        // Destroy texture
        if (imageWrapper != nullptr) {
            FV_MTL_RELEASE(imageWrapper->texture);
        }

        textures.remove(*handle);
    }
}

FvResult MetalWrapper::samplerCreate(FvSampler *sampler,
                                     const FvSamplerCreateInfo *createInfo) {
    FvResult result = FV_RESULT_FAILURE;

    if (sampler != nullptr && createInfo != nullptr) {
        MTLSamplerDescriptor *samplerDescriptor = [MTLSamplerDescriptor new];
        // width
        samplerDescriptor.sAddressMode =
            toMtlSamplerAddressMode(createInfo->addressModeU);
        // height
        samplerDescriptor.tAddressMode =
            toMtlSamplerAddressMode(createInfo->addressModeV);
        // depth
        samplerDescriptor.rAddressMode =
            toMtlSamplerAddressMode(createInfo->addressModeW);
        samplerDescriptor.minFilter = toMtlMinMagFilter(createInfo->minFilter);
        samplerDescriptor.magFilter = toMtlMinMagFilter(createInfo->magFilter);
        samplerDescriptor.mipFilter =
            toMtlSamplerMipFilter(createInfo->mipmapMode);
        samplerDescriptor.lodMinClamp   = createInfo->minLod;
        samplerDescriptor.lodMaxClamp   = createInfo->maxLod;
        samplerDescriptor.maxAnisotropy = createInfo->maxAnisotropy;
        samplerDescriptor.normalizedCoordinates =
            toObjCBool(createInfo->normalizedCoordinates);
        samplerDescriptor.compareFunction =
            toMtlCompareFunction(createInfo->compareFunc);
        samplerDescriptor.borderColor =
            toMtlSamplerBorderColor(createInfo->borderColor);

        id<MTLSamplerState> mtlSampler =
            [device newSamplerStateWithDescriptor:samplerDescriptor];

        FV_MTL_RELEASE(samplerDescriptor); // Done with sampler descriptor

        // Store sampler and return handle
        const Handle *handle = samplers.add(mtlSampler);

        if (handle != nullptr) {
            *sampler = (FvSampler)handle;
            result   = FV_RESULT_SUCCESS;
        }
    }

    return result;
}

void MetalWrapper::samplerDestroy(FvSampler sampler) {
    const Handle *handle = (const Handle *)sampler;

    if (handle != nullptr) {
        id<MTLSamplerState> *mtlSampler = samplers.get(*handle);

        if (mtlSampler != nullptr) {
            // Ensure sampler's memory is freed
            FV_MTL_RELEASE(*mtlSampler);
        }

        // Remove sampler from internal store
        samplers.remove(*handle);
    }
}

FvResult MetalWrapper::graphicsPipelineCreate(
    FvGraphicsPipeline *graphicsPipeline,
    const FvGraphicsPipelineCreateInfo *createInfo) {
    // TODO: Clean up, put into separate functions, deal with optional
    // fields
    // early
    @autoreleasepool {
        FvResult result = FV_RESULT_FAILURE;

        if (graphicsPipeline != nullptr && createInfo != nullptr) {
            // Create GraphicsPipelineWrapper
            GraphicsPipelineWrapper graphicsPipelineWrapper;
            graphicsPipelineWrapper.renderPass          = nullptr;
            graphicsPipelineWrapper.cullMode            = MTLCullModeNone;
            graphicsPipelineWrapper.windingOrder        = MTLWindingClockwise;
            graphicsPipelineWrapper.depthClipMode       = MTLDepthClipModeClip;
            graphicsPipelineWrapper.depthStencilState   = nil;
            graphicsPipelineWrapper.renderPipelineState = nil;
            graphicsPipelineWrapper.vertexInputDescription =
                *createInfo->vertexInputDescription;

            // Get subpass to use from render pass and index
            Handle *renderPassHandle = (Handle *)createInfo->renderPass;

            if (renderPassHandle != nullptr) {
                RenderPassWrapper *renderPassWrapper =
                    renderPasses.get(*renderPassHandle);

                if (renderPassWrapper != nullptr) {
                    SubpassWrapper subpassWrapper =
                        renderPassWrapper->subpasses[createInfo->subpass];

                    // Copy attachment descriptions to graphics pipeline
                    // from
                    // subpass
                    graphicsPipelineWrapper.inputAttachments =
                        subpassWrapper.inputAttachments;
                    graphicsPipelineWrapper.colorAttachments =
                        subpassWrapper.colorAttachments;
                    graphicsPipelineWrapper.depthAttachment =
                        subpassWrapper.depthAttachment;
                    graphicsPipelineWrapper.stencilAttachment =
                        subpassWrapper.stencilAttachment;

                    // Copy render pass descriptor from subpass
                    graphicsPipelineWrapper.renderPass =
                        subpassWrapper.mtlRenderPass;

                    // Fill out shader functions for Metal pipeline
                    // descriptor
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
                                ShaderModuleWrapper *shaderModuleWrapper =
                                    libraries.get(*shaderModuleHandle);

                                if (shaderModuleWrapper != nullptr) {
                                    id<MTLFunction> func = [shaderModuleWrapper
                                                                ->library
                                        newFunctionWithName:
                                            @(shaderStage.entryFunctionName)];

                                    if (func != nil) {
                                        switch (shaderStage.stage) {
                                        case FV_SHADER_STAGE_VERTEX:
                                            mtlPipelineDescriptor
                                                .vertexFunction = func;
                                            result = FV_RESULT_SUCCESS;
                                            break;
                                        case FV_SHADER_STAGE_FRAGMENT:
                                            mtlPipelineDescriptor
                                                .fragmentFunction = func;
                                            result = FV_RESULT_SUCCESS;
                                            break;
                                        default:
                                            break;
                                        }
                                    } else {
                                        printf("Failed to find function with "
                                               "name "
                                               "'%s' in shader module.\n",
                                               shaderStage.entryFunctionName);
                                    }
                                }
                            }
                        }
                    }

                    // Create color blend states for
                    // ColorBlendAttachmentState
                    if (createInfo->colorBlendStateDescription != nullptr &&
                        createInfo->colorBlendStateDescription->attachments !=
                            nullptr) {
                        FvPipelineColorBlendStateDescription colorBlendState =
                            *createInfo->colorBlendStateDescription;

                        // Loop through color attachments and set color
                        // blend
                        // states
                        for (uint32_t i = 0;
                             i < colorBlendState.attachmentCount; ++i) {
                            mtlPipelineDescriptor.colorAttachments[i]
                                .blendingEnabled = toObjCBool(
                                colorBlendState.attachments[i].blendEnable);
                            mtlPipelineDescriptor.colorAttachments[i]
                                .sourceRGBBlendFactor =
                                toMtlBlendFactor(colorBlendState.attachments[i]
                                                     .srcColorBlendFactor);
                            mtlPipelineDescriptor.colorAttachments[i]
                                .destinationRGBBlendFactor =
                                toMtlBlendFactor(colorBlendState.attachments[i]
                                                     .dstColorBlendFactor);
                            mtlPipelineDescriptor.colorAttachments[i]
                                .rgbBlendOperation = toMtlBlendOperation(
                                colorBlendState.attachments[i].colorBlendOp);
                            mtlPipelineDescriptor.colorAttachments[i]
                                .sourceAlphaBlendFactor =
                                toMtlBlendFactor(colorBlendState.attachments[i]
                                                     .srcAlphaBlendFactor);
                            mtlPipelineDescriptor.colorAttachments[i]
                                .destinationAlphaBlendFactor =
                                toMtlBlendFactor(colorBlendState.attachments[i]
                                                     .dstAlphaBlendFactor);
                            mtlPipelineDescriptor.colorAttachments[i]
                                .alphaBlendOperation = toMtlBlendOperation(
                                colorBlendState.attachments[i].alphaBlendOp);
                        }
                    }

                    // Create a vertex descriptor
                    MTLVertexDescriptor *mtlVertexDescriptor =
                        [[MTLVertexDescriptor alloc] init];
                    const FvPipelineVertexInputDescription *vertexInputInfo =
                        createInfo->vertexInputDescription;
                    for (uint32_t iBindingDesc = 0;
                         iBindingDesc <
                         vertexInputInfo->vertexBindingDescriptionCount;
                         ++iBindingDesc) {
                        FvVertexInputBindingDescription bindingDesc =
                            vertexInputInfo
                                ->vertexBindingDescriptions[iBindingDesc];

                        mtlVertexDescriptor.layouts[iBindingDesc].stepFunction =
                            toMtlVertexStepFunction(bindingDesc.inputRate);
                        mtlVertexDescriptor.layouts[iBindingDesc].stride =
                            bindingDesc.stride;
                        // mtlVertexDescriptor.layouts[iBindingDesc].stepRate
                        // =
                        //     1; // Defaults to 1
                    }
                    for (uint32_t iAttributeDesc = 0;
                         iAttributeDesc <
                         vertexInputInfo->vertexAttributeDescriptionCount;
                         ++iAttributeDesc) {
                        FvVertexInputAttributeDescription attributeDesc =
                            vertexInputInfo
                                ->vertexAttributeDescriptions[iAttributeDesc];

                        mtlVertexDescriptor.attributes[iAttributeDesc].format =
                            toMtlVertexFormat(attributeDesc.format);
                        mtlVertexDescriptor.attributes[iAttributeDesc].offset =
                            attributeDesc.offset;
                        mtlVertexDescriptor.attributes[iAttributeDesc]
                            .bufferIndex = attributeDesc.binding;
                    }

                    // Set vertex descriptor
                    mtlPipelineDescriptor.vertexDescriptor =
                        mtlVertexDescriptor;

                    // We've now done all the work required to make a
                    // MTLRenderPipelineState object:
                    NSError *err                                      = nil;
                    MTLRenderPipelineReflection *reflectionInfo       = nil;
                    id<MTLRenderPipelineState> mtlRenderPipelineState = [device
                        newRenderPipelineStateWithDescriptor:
                            mtlPipelineDescriptor
                                                     options:
                                                         MTLPipelineOptionBufferTypeInfo
                                                  reflection:&reflectionInfo
                                                       error:&err];

                    std::vector<ShaderArgument> vertexArguments;
                    for (MTLArgument *arg in reflectionInfo.vertexArguments) {
                        ShaderArgument vertexArgument;

                        vertexArgument.name = [arg.name
                            cStringUsingEncoding:NSUTF8StringEncoding];
                        vertexArgument.index = arg.index;

                        vertexArguments.push_back(vertexArgument);
                    }

                    std::vector<ShaderArgument> fragmentArguments;
                    for (MTLArgument *arg in reflectionInfo.fragmentArguments) {
                        ShaderArgument fragmentArgument;

                        fragmentArgument.name = [arg.name
                            cStringUsingEncoding:NSUTF8StringEncoding];
                        fragmentArgument.index = arg.index;

                        fragmentArguments.push_back(fragmentArgument);
                    }

                    // Loop through the shader modules, get their intermediate
                    // Metal objects and provide them with reflection
                    // information
                    for (uint32_t iShaderModule = 0;
                         iShaderModule < createInfo->stageCount;
                         ++iShaderModule) {
                        Handle *shaderModuleHandle =
                            (Handle *)createInfo->stages[iShaderModule]
                                .shaderModule;

                        ShaderModuleWrapper *shaderModuleWrapper =
                            libraries.get(*shaderModuleHandle);

                        if (shaderModuleWrapper != nullptr) {
                            shaderModuleWrapper->vertexArgumentReflection =
                                vertexArguments;
                            shaderModuleWrapper->fragmentArgumentReflection =
                                fragmentArguments;
                        }
                    }

                    FV_MTL_RELEASE(mtlVertexDescriptor);

                    if (mtlRenderPipelineState != nil && err == nil) {
                        graphicsPipelineWrapper.renderPipelineState =
                            mtlRenderPipelineState;
                    } else {
                        NSString *errString =
                            [NSString stringWithFormat:@"%@", err];

                        printf("Failed to create render pipeline state: %s\n",
                               [errString
                                   cStringUsingEncoding:NSUTF8StringEncoding]);

                        result = FV_RESULT_FAILURE;
                    }

                    MTLDepthStencilDescriptor *mtlDepthStencilDesc =
                        [[MTLDepthStencilDescriptor alloc] init];
                    id<MTLDepthStencilState> depthStencilState = nil;
                    // Create depth stencil states from
                    // DepthStencilDescription
                    if (createInfo->depthStencilDescription != nullptr) {
                        FvPipelineDepthStencilStateDescription
                            depthStencilDesc =
                                *createInfo->depthStencilDescription;

                        mtlDepthStencilDesc.depthCompareFunction =
                            toMtlCompareFunction(
                                depthStencilDesc.depthCompareFunc);
                        mtlDepthStencilDesc.depthWriteEnabled =
                            toObjCBool(depthStencilDesc.depthWriteEnable);

                        if (toObjCBool(depthStencilDesc.stencilTestEnable) ==
                            YES) {
                            mtlDepthStencilDesc.backFaceStencil
                                .stencilFailureOperation =
                                toMtlStencilOperation(
                                    depthStencilDesc.backFaceStencil
                                        .stencilFailOp);
                            mtlDepthStencilDesc.backFaceStencil
                                .depthFailureOperation = toMtlStencilOperation(
                                depthStencilDesc.backFaceStencil.depthFailOp);
                            mtlDepthStencilDesc.backFaceStencil
                                .depthStencilPassOperation =
                                toMtlStencilOperation(
                                    depthStencilDesc.backFaceStencil
                                        .depthStencilPassOp);
                            mtlDepthStencilDesc.backFaceStencil
                                .stencilCompareFunction = toMtlCompareFunction(
                                depthStencilDesc.backFaceStencil
                                    .stencilCompareFunc);
                            mtlDepthStencilDesc.backFaceStencil.readMask =
                                depthStencilDesc.backFaceStencil.readMask;
                            mtlDepthStencilDesc.backFaceStencil.writeMask =
                                depthStencilDesc.backFaceStencil.writeMask;

                            mtlDepthStencilDesc.frontFaceStencil
                                .stencilFailureOperation =
                                toMtlStencilOperation(
                                    depthStencilDesc.frontFaceStencil
                                        .stencilFailOp);
                            mtlDepthStencilDesc.frontFaceStencil
                                .depthFailureOperation = toMtlStencilOperation(
                                depthStencilDesc.frontFaceStencil.depthFailOp);
                            mtlDepthStencilDesc.frontFaceStencil
                                .depthStencilPassOperation =
                                toMtlStencilOperation(
                                    depthStencilDesc.frontFaceStencil
                                        .depthStencilPassOp);
                            mtlDepthStencilDesc.frontFaceStencil
                                .stencilCompareFunction = toMtlCompareFunction(
                                depthStencilDesc.frontFaceStencil
                                    .stencilCompareFunc);
                            mtlDepthStencilDesc.frontFaceStencil.readMask =
                                depthStencilDesc.frontFaceStencil.readMask;
                            mtlDepthStencilDesc.frontFaceStencil.writeMask =
                                depthStencilDesc.frontFaceStencil.writeMask;
                        }
                    }

                    // Create depth stencil state from default
                    // mtlDepthStencilDesc
                    // if no depthStencilDescription provided (Metal
                    // requires a
                    // depth stencil state)
                    depthStencilState = [device
                        newDepthStencilStateWithDescriptor:mtlDepthStencilDesc];

                    // No longer need depthStencil descriptor anymore
                    FV_MTL_RELEASE(mtlDepthStencilDesc);

                    if (depthStencilState != nil) {
                        graphicsPipelineWrapper.depthStencilState =
                            depthStencilState;
                    } else {
                        printf("Failed to create depth stencil state.\n");

                        result = FV_RESULT_FAILURE;
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
                            rasterizer.depthClampEnable == FV_TRUE
                                ? MTLDepthClipModeClamp
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
                            viewportDescription.scissor.origin.x;
                        graphicsPipelineWrapper.scissor.y =
                            viewportDescription.scissor.origin.y;
                        graphicsPipelineWrapper.scissor.width =
                            viewportDescription.scissor.extent.width;
                        graphicsPipelineWrapper.scissor.height =
                            viewportDescription.scissor.extent.height;
                    } else {
                        result = FV_RESULT_FAILURE;
                    }

                    if (createInfo->inputAssemblyDescription != nullptr) {
                        FvPipelineInputAssemblyDescription
                            inputAssemblyDescription =
                                *createInfo->inputAssemblyDescription;

                        // Metal doesn't allow you to turn off primitive
                        // restart
                        // AFAIK
                        if (inputAssemblyDescription.primitiveRestartEnable ==
                            FV_FALSE) {
                            printf("Primitive restart cannot be false in "
                                   "Metal "
                                   "backed.\n");
                            result = FV_RESULT_FAILURE;
                        }

                        graphicsPipelineWrapper.primitiveType =
                            toMtlPrimitiveType(
                                inputAssemblyDescription.primitiveType);
                    } else {
                        result = FV_RESULT_FAILURE;
                    }
                }
            }

            if (result == FV_RESULT_SUCCESS) {
                // Store graphics pipeline wrapper and return handle as
                // graphics
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
}

void MetalWrapper::graphicsPipelineDestroy(
    FvGraphicsPipeline graphicsPipeline) {
    const Handle *handle = (const Handle *)graphicsPipeline;

    if (handle != nullptr) {
        GraphicsPipelineWrapper *pipeline = graphicsPipelines.get(*handle);

        // Destroy pipeline
        if (pipeline != nullptr) {
            FV_MTL_RELEASE(pipeline->depthStencilState);
            FV_MTL_RELEASE(pipeline->renderPipelineState);
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

            // Create subpass wrapper to store subpass information, we
            // cannot
            // complete the analogous Metal structures until we get more
            // information (such as texture and clear values), so we store
            // them
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

                MTLPixelFormat stencilPixelFormat =
                    toMtlPixelFormat(depthStencilAttachment.format);
                if (stencilPixelFormat == MTLPixelFormatDepth24Unorm_Stencil8 ||
                    stencilPixelFormat == MTLPixelFormatDepth32Float_Stencil8) {
                    mtlRenderPipelineDescriptor.stencilAttachmentPixelFormat =
                        stencilPixelFormat;
                }
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
                uint32_t depthStencilAttachmentIndex =
                    subpassDescription.depthStencilAttachment->attachment;

                FvAttachmentDescription depthStencilAttachment =
                    createInfo->attachments[depthStencilAttachmentIndex];

                // Add depth attachment reference
                subpassWrapper.depthAttachment.push_back(
                    *subpassDescription.depthStencilAttachment);

                // Only add stencil attachment reference if using
                MTLPixelFormat stencilPixelFormat =
                    toMtlPixelFormat(depthStencilAttachment.format);
                if (stencilPixelFormat == MTLPixelFormatDepth24Unorm_Stencil8 ||
                    stencilPixelFormat == MTLPixelFormatDepth32Float_Stencil8) {
                    subpassWrapper.stencilAttachment.push_back(
                        *subpassDescription.depthStencilAttachment);
                }
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
        // Get render pass
        RenderPassWrapper *renderPassWrapper = renderPasses.get(*handle);
        if (renderPassWrapper != nullptr) {
            // Get subpasses in render pass
            for (size_t i = 0; i < renderPassWrapper->subpasses.size(); ++i) {
                // Release render pass descriptor and render pipeline
                // descriptor
                FV_MTL_RELEASE(renderPassWrapper->subpasses[i].mtlRenderPass);
                FV_MTL_RELEASE(renderPassWrapper->subpasses[i]
                                   .mtlPipelineDescriptor.vertexFunction);
                FV_MTL_RELEASE(renderPassWrapper->subpasses[i]
                                   .mtlPipelineDescriptor.fragmentFunction);
                FV_MTL_RELEASE(
                    renderPassWrapper->subpasses[i].mtlPipelineDescriptor);
            }
        }

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
        ShaderModuleWrapper shaderModuleWrapper;
        shaderModuleWrapper.library = library;

        if (error == nil) {
            const Handle *handle = libraries.add(shaderModuleWrapper);

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

        FV_MTL_RELEASE(options);
    }

    return result;
}

FvResult MetalWrapper::shaderModuleGetBindingPoint(
    uint32_t *bindingPoint, const FvShaderReflectionRequest *request) {
    if (bindingPoint == nullptr || request == nullptr) {
        return FV_RESULT_FAILURE;
    }

    Handle *shaderModuleHandle = (Handle *)request->shaderModule;

    if (shaderModuleHandle == nullptr) {
        return FV_RESULT_FAILURE;
    }

    const ShaderModuleWrapper *shaderModuleWrapper =
        libraries.get(*shaderModuleHandle);

    if (shaderModuleWrapper == nullptr) {
        return FV_RESULT_FAILURE;
    }

    bool foundArgument = false;
    if (request->shaderStage == FV_SHADER_STAGE_VERTEX) {
        for (uint32_t i = 0;
             i < shaderModuleWrapper->vertexArgumentReflection.size(); ++i) {
            if (shaderModuleWrapper->vertexArgumentReflection[i].name ==
                request->bindingName) {
                *bindingPoint =
                    shaderModuleWrapper->vertexArgumentReflection[i].index;
                foundArgument = true;
            }
        }
    } else if (request->shaderStage == FV_SHADER_STAGE_FRAGMENT) {
        for (uint32_t i = 0;
             i < shaderModuleWrapper->fragmentArgumentReflection.size(); ++i) {
            if (shaderModuleWrapper->fragmentArgumentReflection[i].name ==
                request->bindingName) {
                *bindingPoint =
                    shaderModuleWrapper->fragmentArgumentReflection[i].index;
                foundArgument = true;
            }
        }
    }

    if (foundArgument == false) {
        return FV_RESULT_FAILURE;
    }

    return FV_RESULT_SUCCESS;
}

void MetalWrapper::shaderModuleDestroy(FvShaderModule shaderModule) {
    const Handle *handle = (const Handle *)shaderModule;

    if (handle != nullptr) {
        // Destroy library
        ShaderModuleWrapper *shaderModuleWrapper = libraries.get(*handle);
        if (shaderModuleWrapper != nullptr) {
            FV_MTL_RELEASE(shaderModuleWrapper->library);
        }

        // Remove from handle manager
        libraries.remove(*handle);
    }
}

MTLIndexType MetalWrapper::toMtlIndexType(FvIndexType indexType) {
    MTLIndexType mtlIndexType = MTLIndexTypeUInt32;

    switch (indexType) {
    case FV_INDEX_TYPE_UINT16: {
        mtlIndexType = MTLIndexTypeUInt16;
        break;
    }
    case FV_INDEX_TYPE_UINT32: {
        mtlIndexType = MTLIndexTypeUInt32;
        break;
    }
    default:
        break;
    }

    return mtlIndexType;
}

MTLVertexStepFunction
MetalWrapper::toMtlVertexStepFunction(FvVertexInputRate inputRate) {
    MTLVertexStepFunction stepFunction = MTLVertexStepFunctionConstant;

    switch (inputRate) {
    case FV_VERTEX_INPUT_RATE_VERTEX: {
        stepFunction = MTLVertexStepFunctionPerVertex;
        break;
    }
    case FV_VERTEX_INPUT_RATE_INSTANCE: {
        stepFunction = MTLVertexStepFunctionPerInstance;
        break;
    }
    default:
        break;
    }

    return stepFunction;
}

MTLVertexFormat MetalWrapper::toMtlVertexFormat(FvVertexFormat format) {
    MTLVertexFormat vertexFormat = MTLVertexFormatInvalid;

    switch (format) {
    case FV_VERTEX_FORMAT_UCHAR2: {
        vertexFormat = MTLVertexFormatUChar2;
        break;
    }
    case FV_VERTEX_FORMAT_UCHAR3: {
        vertexFormat = MTLVertexFormatUChar3;
        break;
    }
    case FV_VERTEX_FORMAT_UCHAR4: {
        vertexFormat = MTLVertexFormatUChar4;
        break;
    }
    case FV_VERTEX_FORMAT_CHAR2: {
        vertexFormat = MTLVertexFormatChar2;
        break;
    }
    case FV_VERTEX_FORMAT_CHAR3: {
        vertexFormat = MTLVertexFormatChar3;
        break;
    }
    case FV_VERTEX_FORMAT_CHAR4: {
        vertexFormat = MTLVertexFormatChar4;
        break;
    }
    case FV_VERTEX_FORMAT_USHORT2: {
        vertexFormat = MTLVertexFormatUShort2;
        break;
    }
    case FV_VERTEX_FORMAT_USHORT3: {
        vertexFormat = MTLVertexFormatUShort3;
        break;
    }
    case FV_VERTEX_FORMAT_USHORT4: {
        vertexFormat = MTLVertexFormatUShort4;
        break;
    }
    case FV_VERTEX_FORMAT_SHORT2: {
        vertexFormat = MTLVertexFormatShort2;
        break;
    }
    case FV_VERTEX_FORMAT_SHORT3: {
        vertexFormat = MTLVertexFormatShort3;
        break;
    }
    case FV_VERTEX_FORMAT_SHORT4: {
        vertexFormat = MTLVertexFormatShort4;
        break;
    }
    case FV_VERTEX_FORMAT_HALF2: {
        vertexFormat = MTLVertexFormatHalf2;
        break;
    }
    case FV_VERTEX_FORMAT_HALF3: {
        vertexFormat = MTLVertexFormatHalf3;
        break;
    }
    case FV_VERTEX_FORMAT_HALF4: {
        vertexFormat = MTLVertexFormatHalf4;
        break;
    }
    case FV_VERTEX_FORMAT_FLOAT: {
        vertexFormat = MTLVertexFormatFloat;
        break;
    }
    case FV_VERTEX_FORMAT_FLOAT2: {
        vertexFormat = MTLVertexFormatFloat2;
        break;
    }
    case FV_VERTEX_FORMAT_FLOAT3: {
        vertexFormat = MTLVertexFormatFloat3;
        break;
    }
    case FV_VERTEX_FORMAT_FLOAT4: {
        vertexFormat = MTLVertexFormatFloat4;
        break;
    }
    case FV_VERTEX_FORMAT_INT: {
        vertexFormat = MTLVertexFormatInt;
        break;
    }
    case FV_VERTEX_FORMAT_INT2: {
        vertexFormat = MTLVertexFormatInt2;
        break;
    }
    case FV_VERTEX_FORMAT_INT3: {
        vertexFormat = MTLVertexFormatInt3;
        break;
    }
    case FV_VERTEX_FORMAT_INT4: {
        vertexFormat = MTLVertexFormatInt4;
        break;
    }
    case FV_VERTEX_FORMAT_UINT: {
        vertexFormat = MTLVertexFormatUInt;
        break;
    }
    case FV_VERTEX_FORMAT_UINT2: {
        vertexFormat = MTLVertexFormatUInt2;
        break;
    }
    case FV_VERTEX_FORMAT_UINT3: {
        vertexFormat = MTLVertexFormatUInt3;
        break;
    }
    case FV_VERTEX_FORMAT_UINT4: {
        vertexFormat = MTLVertexFormatUInt4;
        break;
    }
    default:
        break;
    };

    return vertexFormat;
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
    case FV_FORMAT_DEPTH32FLOAT:
        pixelFormat = MTLPixelFormatDepth32Float;
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
    case FV_BLEND_OP_ADD:
        blendOperation = MTLBlendOperationAdd;
        break;
    case FV_BLEND_OP_SUBTRACT:
        blendOperation = MTLBlendOperationSubtract;
        break;
    case FV_BLEND_OP_REVERSE_SUBTRACT:
        blendOperation = MTLBlendOperationReverseSubtract;
        break;
    case FV_BLEND_OP_MIN:
        blendOperation = MTLBlendOperationMin;
        break;
    case FV_BLEND_OP_MAX:
        blendOperation = MTLBlendOperationMax;
        break;
    default:
        break;
    }

    return blendOperation;
}

BOOL MetalWrapper::toObjCBool(FvBool b) { return (b == FV_FALSE ? NO : YES); }

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

MTLPrimitiveType
MetalWrapper::toMtlPrimitiveType(FvPrimitiveType primitiveType) {
    MTLPrimitiveType type = MTLPrimitiveTypeTriangle;

    switch (primitiveType) {
    case FV_PRIMITIVE_TYPE_POINT_LIST:
        type = MTLPrimitiveTypePoint;
        break;
    case FV_PRIMITIVE_TYPE_LINE_LIST:
        type = MTLPrimitiveTypeLine;
        break;
    case FV_PRIMITIVE_TYPE_LINE_STRIP:
        type = MTLPrimitiveTypeLineStrip;
        break;
    case FV_PRIMITIVE_TYPE_TRIANGLE_LIST:
        type = MTLPrimitiveTypeTriangle;
        break;
    case FV_PRIMITIVE_TYPE_TRIANGLE_STRIP:
        type = MTLPrimitiveTypeTriangleStrip;
        break;
    default:
        break;
    }

    return type;
}

MTLSamplerAddressMode
MetalWrapper::toMtlSamplerAddressMode(FvSamplerAddressMode addressMode) {
    MTLSamplerAddressMode mtlSamplerAddressMode =
        MTLSamplerAddressModeClampToEdge;

    switch (addressMode) {
    case FV_SAMPLER_ADDRESS_MODE_REPEAT: {
        mtlSamplerAddressMode = MTLSamplerAddressModeRepeat;
        break;
    }
    case FV_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: {
        mtlSamplerAddressMode = MTLSamplerAddressModeMirrorRepeat;
        break;
    }
    case FV_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: {
        mtlSamplerAddressMode = MTLSamplerAddressModeClampToEdge;
        break;
    }
    case FV_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: {
        mtlSamplerAddressMode = MTLSamplerAddressModeClampToBorderColor;
        break;
    }
    case FV_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: {
        mtlSamplerAddressMode = MTLSamplerAddressModeMirrorClampToEdge;
        break;
    }
    default:
        break;
    }

    return mtlSamplerAddressMode;
}

MTLSamplerMinMagFilter MetalWrapper::toMtlMinMagFilter(FvMinMagFilter filter) {
    MTLSamplerMinMagFilter mtlSamplerMinMagFilter =
        MTLSamplerMinMagFilterNearest;

    switch (filter) {
    case FV_MIN_MAG_FILTER_NEAREST: {
        mtlSamplerMinMagFilter = MTLSamplerMinMagFilterNearest;
        break;
    }
    case FV_MIN_MAG_FILTER_LINEAR: {
        mtlSamplerMinMagFilter = MTLSamplerMinMagFilterLinear;
        break;
    }
    default:
        break;
    }

    return mtlSamplerMinMagFilter;
}

MTLSamplerMipFilter
MetalWrapper::toMtlSamplerMipFilter(FvSamplerMipmapMode mipmapMode) {
    MTLSamplerMipFilter mtlSamplerMipFilter = MTLSamplerMipFilterNearest;

    switch (mipmapMode) {
    case FV_SAMPLER_MIPMAP_MODE_NEAREST: {
        mtlSamplerMipFilter = MTLSamplerMipFilterNearest;
        break;
    }
    case FV_SAMPLER_MIPMAP_MODE_LINEAR: {
        mtlSamplerMipFilter = MTLSamplerMipFilterLinear;
        break;
    }
    default:
        break;
    }

    return mtlSamplerMipFilter;
}

MTLSamplerBorderColor
MetalWrapper::toMtlSamplerBorderColor(FvBorderColor borderColor) {
    MTLSamplerBorderColor mtlSamplerBorderColor =
        MTLSamplerBorderColorOpaqueBlack;

    switch (borderColor) {
    case FV_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK: // Intentional fallthru
    case FV_BORDER_COLOR_INT_TRANSPARENT_BLACK:
        mtlSamplerBorderColor = MTLSamplerBorderColorTransparentBlack;
        break;
    case FV_BORDER_COLOR_FLOAT_OPAQUE_BLACK: // Intentional fallthru
    case FV_BORDER_COLOR_INT_OPAQUE_BLACK:
        mtlSamplerBorderColor = MTLSamplerBorderColorOpaqueBlack;
        break;
    case FV_BORDER_COLOR_FLOAT_OPAQUE_WHITE: // Intentional fallthru
    case FV_BORDER_COLOR_INT_OPAQUE_WHITE:
        mtlSamplerBorderColor = MTLSamplerBorderColorOpaqueWhite;
        break;
    default:
        break;
    }

    return mtlSamplerBorderColor;
}
}
