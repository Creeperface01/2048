#ifndef INC_2048_SDL_MENU_H
#define INC_2048_SDL_MENU_H

#include "sdl_utils.h"
#include "sdl_main.h"

#define MENU_WIDTH 300
#define MENU_HEIGHT 500

#define BUTTON_HEIGHT 30
#define BUTTON_WIDTH 200

#define BUTTON_COLOR 0xd4b274ffu
#define SELECT_COLOR 0xf75348ffu

window_handle_t *create_menu_window(sdl_data_t *data);

#endif //INC_2048_SDL_MENU_H
