#pragma once

#include <Fever/Fever.h>
#include <Fever/FeverPlatform.h>

#if FV_PLATFORM_MACOS
typedef struct FvMacOSSurfaceCreateInfo {
    void *nsWindow;
} FvMacOSSurfaceCreateInfo;

extern FvResult
fvCreateMacOSSurface(FvSurface *surface,
                     const FvMacOSSurfaceCreateInfo *createInfo);
#endif
