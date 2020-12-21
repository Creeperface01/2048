#ifndef PROJEKT1_GAME_SDL_HANDLER_H
#define PROJEKT1_GAME_SDL_HANDLER_H

#include "game.h"
#include "sdl_utils.h"
#include "SDL_ttf.h"

typedef struct {
    window_handle_t *handle;
    SDL_Texture *texture;
    SDL_Texture **textures;
    bool changed;
    round_result_t *result;
    game_t *game;
    bool end;
} sdl_game_t;

typedef struct animation_t {
    void (*handler)(sdl_game_t *, struct animation_t *, int);

    void (*finishHandler)(sdl_game_t *, struct animation_t *);

    void *data;

    Uint32 start;
    Sint32 fromValue;
    Sint32 toValue;
    Uint32 duration; //ms
} animation_t;

window_handle_t *game_sdl_init(sdl_data_t *data, game_t *game);

void game_sdl_destroy(window_handle_t *handle);

//void game_sdl_start(sdl_game_t *sdl_game);

void game_sdl_render_tiles(sdl_game_t *sdl_game);

#endif //PROJEKT1_GAME_SDL_HANDLER_H
