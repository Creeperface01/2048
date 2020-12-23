#include "sdl_menu.h"
#include "gfx/SDL2_gfxPrimitives.h"
#include <stdbool.h>

#define SELECT_KEY "selected"

typedef struct {
    uint8_t selected;
} menu_t;

uint8_t getMouseBtn(int32_t x, int32_t y) {
    uint16_t btnStart = (MENU_WIDTH - BUTTON_WIDTH) / 2;

    if (x >= btnStart && x <= MENU_WIDTH - btnStart) {

        for (int i = 0; i < 5; ++i) {
            uint16_t y0 = 100 + (70 * i);
            uint16_t y1 = 150 + (70 * i);

            if (i == 4) {
                y0 += 50;
                y1 += 50;
            }

            if (y >= y0 && y <= y1) {
                return i;
            }
        }
    }

    return -1;
}

void handleEvent(window_handle_t *handle, SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {

    }

    if (event->type == SDL_MOUSEMOTION) {
        menu_t *menu = SDL_GetWindowData(handle->window, SELECT_KEY);

        uint8_t selected0 = menu->selected;
        menu->selected = getMouseBtn(event->motion.x, event->motion.y);

        if (menu->selected != selected0) {
            if (menu->selected < 5) {
                sdl_main_set_hand_cursor(handle->data);
            } else {
                sdl_main_reset_cursor(handle->data);
            }
        }
    }

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        if (event->button.button != 1) {
            return;
        }

        uint8_t btn = getMouseBtn(event->button.x, event->button.y);
        if (btn < 5) {
            sdl_main_reset_cursor(handle->data);
            switch (btn) {
                case 0:
                    sdl_main_start_game(
                            handle->data,
                            sdl_main_get_width(handle->data),
                            sdl_main_get_height(handle->data)
                    );
                    break;
                case 1:
                    sdl_main_start_game(handle->data, 4, 4);
                    break;
                case 2:
                    sdl_main_start_game(handle->data, 6, 6);
                    break;
                case 3:
                    sdl_main_start_game(handle->data, 8, 8);
                    break;
                case 4:
                    sdl_main_open_scores(handle->data);
                    break;
                default:
                    break;
            }
        }
    }
}

void onClose(window_handle_t *handle) {
    free(SDL_GetWindowData(handle->window, SELECT_KEY));
}

void render(window_handle_t *handle) {
    SDL_SetRenderDrawColor(handle->renderer, 255, 255, 255, 255);
    SDL_RenderClear(handle->renderer);

    menu_t *menu = SDL_GetWindowData(handle->window, SELECT_KEY);

    render_text(handle, (SDL_Color) {0xED, 0xC2, 0x2D, 255}, "2048", MENU_WIDTH / 2, 10, 70, true);

    char *text[] = {
            "DEFAULT",
            "SMALL",
            "MEDIUM",
            "LARGE",
            "HIGH SCORES"
    };

    uint16_t x0 = (MENU_WIDTH - BUTTON_WIDTH) / 2;
    uint16_t x1 = MENU_WIDTH - x0;

    for (int i = 0; i < 5; ++i) {
        uint16_t y0 = 100 + (i * 70);
        uint16_t y1 = 150 + (i * 70);

        if (i == 4) {
            y0 += 50;
            y1 += 50;
        }

        roundedBoxColor(handle->renderer,
                        x0,
                        y0,
                        x1,
                        y1,
                        10,
                        BUTTON_COLOR
        );

        if (menu->selected == i) {
            render_text(handle, COLOR_WHITE, text[i], MENU_WIDTH / 2, y0 + 10, 30, true);
        } else {
            render_text(handle, COLOR_BLACK, text[i], MENU_WIDTH / 2, y0 + 10, 30, true);
        }
    }

    SDL_RenderPresent(handle->renderer);
}

window_handle_t *create_menu_window(sdl_data_t *data) {
    window_handle_t *handle = create_window(WINDOW_MAIN,
                                            data,
                                            "2048",
                                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            MENU_WIDTH, MENU_HEIGHT,
                                            SDL_WINDOW_SHOWN);

    handle->eventHandler = handleEvent;
    handle->renderHandler = render;
    handle->closeHandler = onClose;

    menu_t *menu = malloc(sizeof(menu_t));
    menu->selected = -1;

    SDL_SetWindowData(handle->window, SELECT_KEY, menu);

    return handle;
}