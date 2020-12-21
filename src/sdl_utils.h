#ifndef PROJEKT1_UTILS_H
#define PROJEKT1_UTILS_H

#include "SDL.h"
#include "data.h"
#include "config.h"
#include "sdl_main.h"
#include <stdbool.h>

#define WINDOW_HANDLE_DATA_KEY "window_handle"

typedef enum {
    WINDOW_MAIN,
    WINDOW_GAME,
    WINDOW_SCORES
} window_type;

typedef struct window_handle_t {
    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_data_t *data;
    window_type type;

    void (*renderHandler)(struct window_handle_t *);

    void (*eventHandler)(struct window_handle_t *, SDL_Event *);

    void (*closeHandler)(struct window_handle_t *);
} window_handle_t;

window_handle_t *get_window_handle(SDL_Window *window);

window_handle_t *get_window_handle_from_id(uint32_t windowId);

window_handle_t *
create_window(window_type type, sdl_data_t *data, const char *title, int x, int y, int w, int h, Uint32 flags);

void destroy_window(SDL_Window *window);

int SDL_SetRenderDrawColorRGB(SDL_Renderer *renderer, Uint32 rgb, Uint8 a);

int SDL_SetRenderDrawColorRGBA(SDL_Renderer *renderer, Uint32 rgba);

SDL_Color color_from_rgb(Uint32 rgb);

uint8_t *new_uint8(uint8_t a);

void resize_texture_by_height(SDL_Rect *rect, SDL_Texture *texture, int max_height);

void render_text(window_handle_t *handle, SDL_Color color, char *text, int x, int y, int max_height, bool center);

#endif //PROJEKT1_UTILS_H
