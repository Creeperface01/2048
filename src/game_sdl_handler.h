#ifndef PROJEKT1_GAME_SDL_HANDLER_H
#define PROJEKT1_GAME_SDL_HANDLER_H

#include "game.h"
#include "utils.h"
#include "SDL_ttf.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Texture **textures;
    int changed;
    game_t *game;
} sdl_game_t;

sdl_game_t *game_sdl_init(game_t *game);

void game_sdl_destroy(sdl_game_t *sdl_game);

void game_sdl_start(sdl_game_t *sdl_game);

void game_sdl_render_tiles(sdl_game_t *sdl_game);

#endif //PROJEKT1_GAME_SDL_HANDLER_H
