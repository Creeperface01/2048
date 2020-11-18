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

int SDL_RenderFillRectRounded(SDL_Renderer *renderer, const SDL_Rect *rect, Uint32 rgb, Uint8 a, int radius) {

}

void vec_game2sdl(vec2i_t *sdl_vec, vec2i_t *game_vec) {
    sdl_vec->x = game_vec->x;
    sdl_vec->y = WINDOW_HEIGHT - game_vec->y;
}