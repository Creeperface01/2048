#ifndef INC_2048_SDL_SCORE_H
#define INC_2048_SDL_SCORE_H

#include "sdl_utils.h"
#include <stdint.h>

void sdl_score_add(uint16_t score);

window_handle_t *sdl_score_create_window(sdl_data_t *data);

#endif //INC_2048_SDL_SCORE_H
