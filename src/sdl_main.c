#include "sdl_main.h"
#include "sdl_utils.h"
#include "SDL_ttf.h"
#include "sdl_menu.h"
#include "sdl_assets.h"
#include "sdl_game.h"
#include "sdl_score.h"
#include <stdbool.h>

typedef struct sdl_data_t {
    window_handle_t *menuWindow;
    window_handle_t *currentWindow;
    TTF_Font *font;
    SDL_Cursor *cursor;
    uint8_t defaultWidth;
    uint8_t defaultHeight;
} sdl_data_t;

void closeWindow(window_handle_t *handle) {
    if (handle->closeHandler != NULL) {
        handle->closeHandler(handle);
    }

    handle->data->currentWindow = handle->data->menuWindow;
    destroy_window(handle->window);
}

bool handle_window_event(window_handle_t *handle, SDL_WindowEvent *windowEvent) {
    if (windowEvent->event == SDL_WINDOWEVENT_CLOSE) {
        closeWindow(handle);
        return false;
    }
    return true;
}

void run(sdl_data_t *data) {
    bool quit = false;

    while (true) {

        window_handle_t *handle = data->currentWindow;
        bool main_window = handle->type == WINDOW_MAIN;

        if (handle == NULL) {
            continue;
        }

        //handle events
        SDL_Event e;
        while (!quit && SDL_PollEvent(&e)) {

            switch (e.type) {
                case SDL_QUIT:
                    closeWindow(handle);
                    quit = true;
                    break;
                case SDL_WINDOWEVENT:
                    if (e.window.windowID != SDL_GetWindowID(handle->window)) {
                        continue;
                    }

                    if (!handle_window_event(handle, &e.window)) {
                        quit = true;
                        break;
                    }
                default:
                    if (handle->eventHandler != NULL) {
                        handle->eventHandler(handle, &e);
                    }
            }
        }

        if (quit) {
            if (main_window) {
                break;
            }

            quit = false;
            continue;
        }

        if (handle->renderHandler != NULL) {
            handle->renderHandler(handle);
        }
    }
}

void sdl_main_start_game(sdl_data_t *data, uint8_t width, uint8_t height) {
    game_t *game = game_create(width, height);

    window_handle_t *handle = game_sdl_init(data, game);
    data->currentWindow = handle;
}

void sdl_main_open_scores(sdl_data_t *data) {
    window_handle_t *handle = sdl_score_create_window(data);
    data->currentWindow = handle;
}

TTF_Font *sdl_main_get_font(sdl_data_t *data) {
    return data->font;
}

uint8_t sdl_main_get_width(sdl_data_t *data) {
    return data->defaultWidth;
}

uint8_t sdl_main_get_height(sdl_data_t *data) {
    return data->defaultHeight;
}

void sdl_main_set_hand_cursor(sdl_data_t *data) {
    SDL_SetCursor(data->cursor);
}

void sdl_main_reset_cursor(sdl_data_t *data) {
    SDL_SetCursor(SDL_GetDefaultCursor());
}

sdl_data_t *sdl_main_init(uint16_t width, uint16_t height) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    sdl_data_t *data = malloc(sizeof(sdl_data_t));
    data->font = load_font(500);
    data->currentWindow = NULL;
    data->defaultHeight = height;
    data->defaultWidth = width;
    data->cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    window_handle_t *handle = create_menu_window(data);
    data->currentWindow = data->menuWindow = handle;

    run(data);

    TTF_CloseFont(data->font);
    SDL_FreeCursor(data->cursor);

    data->font = NULL;
    data->cursor = NULL;
    data->currentWindow = NULL;
    data->menuWindow = NULL;
    free(data);

    SDL_Quit();
}