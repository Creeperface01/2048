#include "sdl_utils.h"

window_handle_t *create_window_handle(SDL_Window *window, SDL_Renderer *renderer, sdl_data_t *data, window_type type) {
    window_handle_t *handle = malloc(sizeof(window_handle_t));

    handle->type = type;
    handle->window = window;
    handle->renderer = renderer;
    handle->data = data;
    handle->closeHandler = NULL;
    handle->renderHandler = NULL;
    handle->eventHandler = NULL;

    return handle;
}

window_handle_t *get_window_handle(SDL_Window *window) {
    return SDL_GetWindowData(window, WINDOW_HANDLE_DATA_KEY);
}

window_handle_t *get_window_handle_from_id(uint32_t windowId) {
    SDL_Window *window = SDL_GetWindowFromID(windowId);

    if (window == NULL) {
        return NULL;
    }

    return get_window_handle(window);
}

window_handle_t *
create_window(window_type type, sdl_data_t *data, const char *title, int x, int y, int w, int h, Uint32 flags) {
    SDL_Window *window = SDL_CreateWindow(title, x, y, w, h, flags);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);


    window_handle_t *handle = create_window_handle(window, renderer, data, type);
    SDL_SetWindowData(window, WINDOW_HANDLE_DATA_KEY, handle);

    SDL_SetRenderDrawColor(handle->renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(handle->renderer);

    return handle;
}

void destroy_window(SDL_Window *window) {
    window_handle_t *handle = get_window_handle(window);

    SDL_DestroyRenderer(handle->renderer);
    SDL_DestroyWindow(window);

    handle->renderer = NULL;
    handle->window = NULL;
    handle->eventHandler = NULL;
    handle->renderHandler = NULL;
    handle->closeHandler = NULL;

    free(handle);
}

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

SDL_Color color_from_rgb(Uint32 rgb) {
    return (SDL_Color) {
            (rgb >> 16u) & 0xffu,
            (rgb >> 8u) & 0xffu,
            rgb & 0xffu
    };
}

uint8_t *new_uint8(uint8_t a) {
    uint8_t *ptr = malloc(sizeof(uint8_t));
    *ptr = a;

    return ptr;
}

void resize_texture_by_height(SDL_Rect *rect, SDL_Texture *texture, int max_height) {
    int originHeight;
    int originWidth;

    SDL_QueryTexture(texture, NULL, NULL, &originWidth, &originHeight);

    int height = min(max_height, originHeight);
    int width = (height * originWidth) / originHeight;

    rect->h = height;
    rect->w = width;
}

void render_text(window_handle_t *handle, SDL_Color color, char *text, int x, int y, int max_height, bool center) {
    SDL_Surface *surf = TTF_RenderText_Blended(sdl_main_get_font(handle->data), text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(handle->renderer, surf);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;

    resize_texture_by_height(&rect, texture, max_height);

    if (center) {
        rect.x -= rect.w / 2;
    }

    SDL_RenderCopy(handle->renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surf);
}