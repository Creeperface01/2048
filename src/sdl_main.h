#ifndef INC_2048_SDL_MAIN_H
#define INC_2048_SDL_MAIN_H

#include "SDL.h"
#include "SDL_ttf.h"
#include "game.h"

typedef struct sdl_data_t sdl_data_t;

sdl_data_t *sdl_main_init(uint16_t width, uint16_t height);

TTF_Font *sdl_main_get_font(sdl_data_t *data);

void sdl_main_start_game(sdl_data_t *data, uint8_t width, uint8_t height);

uint8_t sdl_main_get_width(sdl_data_t *data);

uint8_t sdl_main_get_height(sdl_data_t *data);

void sdl_main_set_hand_cursor(sdl_data_t *data);

void sdl_main_reset_cursor(sdl_data_t *data);

void sdl_main_open_scores(sdl_data_t *data);


#endif //INC_2048_SDL_MAIN_H
