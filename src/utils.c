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

void SDL_DrawCircle(SDL_Renderer *renderer, int centreX, int centreY, int radius) {
    const int diameter = (radius * 2);

    int x = (radius - 1);
    int y = 0;
    int tx = 1;
    int ty = 1;
    int error = (tx - diameter);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

int SDL_RenderFillRectRounded(SDL_Renderer *renderer, const SDL_Rect *rect, int radius) {
    SDL_DrawCircle(renderer, rect->x + radius, rect->y + radius, radius);
    SDL_DrawCircle(renderer, rect->x + radius, (rect->y + rect->h) - radius, radius);

    SDL_DrawCircle(renderer, (rect->x + rect->w) - radius, rect->y + radius, radius);
    SDL_DrawCircle(renderer, (rect->x + rect->w) - radius, (rect->y + rect->h) - radius, radius);

    SDL_Rect temp;
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