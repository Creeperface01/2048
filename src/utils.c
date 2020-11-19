#include "utils.h"

int SDL_SetRenderDrawColorRGBA(SDL_Renderer *renderer, Uint32 rgba) {
    return SDL_SetRenderDrawColor(
            renderer,
            (rgba >> 24u) & 0xffu,
            (rgba >> 16u) & 0xffu,
            (rgba >> 8u) & 0xffu,
            rgba & 0xffu
    );
}

int SDL_SetRenderDrawColorRGB(SDL_Renderer *renderer, Uint32 rgb, Uint8 a) {
    return SDL_SetRenderDrawColorRGBA(renderer, (rgb << 8u) | a);
}

void vec_game2sdl(vec2i_t *sdl_vec, vec2i_t *game_vec) {
    sdl_vec->x = game_vec->x;
    sdl_vec->y = WINDOW_HEIGHT - game_vec->y;
}

SDL_Color color_from_rgb(Uint32 rgb) {
    return (SDL_Color) {
            (rgb >> 16u) & 0xffu,
            (rgb >> 8u) & 0xffu,
            rgb & 0xffu
    };
}