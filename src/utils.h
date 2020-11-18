#ifndef PROJEKT1_UTILS_H
#define PROJEKT1_UTILS_H

#include "SDL.h"
#include "data.h"
#include "config.h"

int SDL_SetRenderDrawColorRGB(SDL_Renderer *renderer, Uint32 rgb, Uint8 a);

int SDL_SetRenderDrawColorRGBA(SDL_Renderer *renderer, Uint32 rgba);

void vec_game2sdl(vec2i_t *sdl_vec, vec2i_t *game_vec);

SDL_Color color_from_rgb(Uint32 rgb);

//void rect_of(SDL_Rect *rect, int x, int y);

//void rect_of_vec(SDL_Rect *rect, vec2i_t *vec1, vec2i_t *vec2);


#endif //PROJEKT1_UTILS_H
