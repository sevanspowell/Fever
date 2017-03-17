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
}
