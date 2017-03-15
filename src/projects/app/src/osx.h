#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <Fever/Fever.h>

FvResult getOSXSurface(FvSurface *surface, SDL_SysWMinfo windowInfo);
