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
    MTL_RELEASE(commandQueue);
    MTL_RELEASE(device);
}
}
