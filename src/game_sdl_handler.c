#include "game_sdl_handler.h"
#include "stdlib.h"
#include "stdio.h"

sdl_game_t *game_sdl_init(game_t *game) {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);

    sdl_game_t *sdl_game = malloc(sizeof(sdl_game_t));

    sdl_game->changed = 1;
    sdl_game->window = SDL_CreateWindow(
            "2048",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN
    );

    sdl_game->game = game;
    sdl_game->renderer = SDL_CreateRenderer(sdl_game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    return sdl_game;
}

void game_sdl_destroy(sdl_game_t *sdl_game) {
    SDL_DestroyWindow(sdl_game->window);
    SDL_DestroyRenderer(sdl_game->renderer);
    SDL_Quit();

    sdl_game->window = NULL;
    sdl_game->renderer = NULL;
    free(sdl_game);
}

Uint8 isArrowKey(SDL_KeyCode code) {
    return code >= SDLK_RIGHT && code <= SDLK_UP;
}

direction_t key2dir(SDL_KeyCode code) {
    switch (code) {
        case SDLK_DOWN:
            return DIRECTION_DOWN;
        case SDLK_UP:
            return DIRECTION_UP;
        case SDLK_RIGHT:
            return DIRECTION_RIGHT;
        case SDLK_LEFT:
        default:
            return DIRECTION_LEFT;
    }
}

void rect_of(SDL_Rect *rect, int x, int y) {
    rect->x = x;
    rect->y = ((int) WINDOW_HEIGHT - y) - TILE_SIZE;

    rect->w = TILE_SIZE;
    rect->h = TILE_SIZE;
}

void game_sdl_render_tiles(sdl_game_t *sdl_game) {
    for (int x = 0; x < C; ++x) {
        for (int y = 0; y < R; ++y) {
            vec2i_t vec = vec2i_of(x, y);
            tile_t *tile = game_get_tile(sdl_game->game, &vec);

            Uint32 color;
            if (tile == NULL) {
                color = COLOR_EMPTY;
            } else {
                color = tile->color;
            }

            int minX = SPACING + (x * TILE_SIZE) + (x * SPACING);
            int minY = SPACING + (y * TILE_SIZE) + (y * SPACING);

            SDL_Rect rect;
            rect_of(&rect, minX, minY);

            SDL_SetRenderDrawColorRGB(sdl_game->renderer, color, 255);
            SDL_RenderFillRect(sdl_game->renderer, &rect);
        }
    }
}

void game_sdl_start(sdl_game_t *sdl_game) {
    SDL_Event e;
    int quit = 0;
    Uint32 frame_start, frame_time, frame_delay = 1000;

    while (!quit) {
        frame_start = SDL_GetTicks();
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
                break;
            }

            if (e.type == SDL_KEYDOWN) {
                SDL_KeyCode key = e.key.keysym.sym;

                if (isArrowKey(key)) {
                    game_handle_move(sdl_game->game, key2dir(key));
                    sdl_game->changed = 1;
                } else if (key == SDLK_ESCAPE) {
                    quit = 1;
                    break;
                }
            }
        }

        if (sdl_game->changed) {
            SDL_SetRenderDrawColorRGB(sdl_game->renderer, COLOR_BACKGROUND, 255);
            SDL_RenderClear(sdl_game->renderer);

            game_sdl_render_tiles(sdl_game);

//            SDL_Rect rect = {10, 10, 200, 200};
//
//            SDL_SetRenderDrawColorRGB(sdl_game->renderer, COLOR_1024, 255);
//            SDL_RenderFillRect(sdl_game->renderer, &rect);

            sdl_game->changed = 0;
        }

        SDL_RenderPresent(sdl_game->renderer);

//        frame_time = SDL_GetTicks() - frame_start;

//        if (frame_delay > frame_time) {
//            SDL_Delay(frame_delay - frame_time);
//        }
    }
}
