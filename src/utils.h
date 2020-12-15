#ifndef PROJEKT1_UTILS_H
#define PROJEKT1_UTILS_H

#include "SDL.h"
#include "data.h"
#include "config.h"

int SDL_SetRenderDrawColorRGB(SDL_Renderer *renderer, Uint32 rgb, Uint8 a);

int SDL_SetRenderDrawColorRGBA(SDL_Renderer *renderer, Uint32 rgba);

SDL_Color color_from_rgb(Uint32 rgb);

#endif //PROJEKT1_UTILS_H
