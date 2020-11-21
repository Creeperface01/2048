#include "game_sdl_handler.h"
#include "stdlib.h"
#include "stdio.h"
#include "gfx/SDL2_gfxPrimitives.h"

void resize_texture_by_height(SDL_Rect *rect, SDL_Texture *texture, int max_height) {
    int originHeight;
    int originWidth;

    SDL_QueryTexture(texture, NULL, NULL, &originWidth, &originHeight);

    int height = min(max_height, originHeight);
    int width = (height * originWidth) / originHeight;

    rect->h = height;
    rect->w = width;
}

SDL_Texture *create_tile_texture(sdl_game_t *sdl_game, tile_t *tile) {
    float texture_size = TILE_SIZE * MERGE_ANIMATION_SCALE;

    SDL_Texture *texture = SDL_CreateTexture(sdl_game->renderer, SDL_GetWindowPixelFormat(sdl_game->window),
                                             SDL_TEXTUREACCESS_TARGET,
                                             (int) texture_size, (int) texture_size);
    SDL_SetRenderTarget(sdl_game->renderer, texture);

    SDL_SetRenderDrawColorRGB(sdl_game->renderer, COLOR_BACKGROUND, 255);
    SDL_RenderClear(sdl_game->renderer);

    Uint8 *color_bytes;

    if (tile != NULL) {
        color_bytes = (Uint8 *) &tile->color;
    } else {
        int tmp = COLOR_EMPTY;
        color_bytes = (Uint8 *) &tmp;
    }

    roundedBoxRGBA(sdl_game->renderer, 0, 0, texture_size - 1, texture_size - 1, 5, color_bytes[2],
                   color_bytes[1], color_bytes[0], 0xff);

    if (tile != NULL) { //render value
        SDL_Color color;

        if (tile->value > 4) {
            color = color_from_rgb(COLOR_NUMBER_LIGHT);
        } else {
            color = color_from_rgb(COLOR_NUMBER_DARK);
        }

        char text[10];
        snprintf(text, 10, "%d", (int) tile->value);

        SDL_Surface *surf = TTF_RenderText_Blended(sdl_game->font, text, color);
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(sdl_game->renderer, surf);

        int maxHeight = (int) (texture_size * 0.8f);
        int maxWidth = (int) (texture_size - (texture_size * 0.2f));

        int originHeight;
        int originWidth;

        SDL_QueryTexture(text_texture, NULL, NULL, &originWidth, &originHeight);

        int height = min(maxHeight, (originHeight * maxWidth) / originWidth);
        int width = (height * originWidth) / originHeight;

        SDL_Rect rect = {(int) ((texture_size - width) / 2), (int) ((texture_size - height) / 2), width, height};
        SDL_RenderCopy(sdl_game->renderer, text_texture, NULL, &rect);

        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(surf);
    }

    return texture;
}

sdl_game_t *game_sdl_init(game_t *game) {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    sdl_game_t *sdl_game = malloc(sizeof(sdl_game_t));

    sdl_game->changed = 1;
    sdl_game->window = SDL_CreateWindow(
            "2048",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN
    );

    sdl_game->game = game;
    sdl_game->renderer = SDL_CreateRenderer(sdl_game->window, -1,
                                            SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    sdl_game->texture = SDL_CreateTexture(sdl_game->renderer, SDL_GetWindowPixelFormat(sdl_game->window),
                                          SDL_TEXTUREACCESS_TARGET,
                                          WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetRenderTarget(sdl_game->renderer, sdl_game->texture);

    sdl_game->font = TTF_OpenFont("assets/OpenSans-Bold.ttf", 500);

    sdl_game->textures = malloc(sizeof(SDL_Texture *) * (COLORS_COUNT + 1));

    for (unsigned int i = 0; i < COLORS_COUNT; i++) {
        sdl_game->textures[i] = create_tile_texture(sdl_game, &sdl_game->game->tile_types[i]);
    }

    sdl_game->textures[COLORS_COUNT] = create_tile_texture(sdl_game, NULL);

    SDL_SetRenderTarget(sdl_game->renderer, sdl_game->texture);
    return sdl_game;
}

void game_sdl_destroy(sdl_game_t *sdl_game) {
    TTF_CloseFont(sdl_game->font);
    SDL_DestroyWindow(sdl_game->window);
    SDL_DestroyRenderer(sdl_game->renderer);
    SDL_DestroyTexture(sdl_game->texture);
    SDL_Quit();

    sdl_game->window = NULL;
    sdl_game->renderer = NULL;
    sdl_game->texture = NULL;
    sdl_game->font = NULL;
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

void game_sdl_render_tile(sdl_game_t *sdl_game, int x, int y, tile_t *tile) {
    x = SPACING + (x * TILE_SIZE) + (x * SPACING);
    y = SPACING + (y * TILE_SIZE) + (y * SPACING);

    SDL_Texture *texture = sdl_game->textures[tile != NULL ? tile->index : COLORS_COUNT];
    SDL_Rect rect;
    rect.x = x;
    rect.y = ((int) WINDOW_HEIGHT - y) - TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;

    SDL_RenderCopy(sdl_game->renderer, texture, NULL, &rect);
}

void game_sdl_render_tiles(sdl_game_t *sdl_game) {
    vec2i_t vec;

    for (int x = 0; x < C; ++x) {
        for (int y = 0; y < R; ++y) {
            vec.x = x;
            vec.y = y;
            game_sdl_render_tile(sdl_game, x, y, game_get_tile(sdl_game->game, &vec));
        }
    }
}

void render_text(sdl_game_t *sdl_game, SDL_Color color, char *text, int x, int y, int max_height) {
    SDL_Surface *surf = TTF_RenderText_Blended(sdl_game->font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(sdl_game->renderer, surf);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;

    resize_texture_by_height(&rect, texture, max_height);

    SDL_RenderCopy(sdl_game->renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surf);
}

void game_sdl_render(sdl_game_t *sdl_game, SDL_bool tiles) {
    SDL_SetRenderDrawColorRGB(sdl_game->renderer, COLOR_BACKGROUND, 255);
    SDL_RenderClear(sdl_game->renderer);

    SDL_Rect rect = {0, 0, WINDOW_WIDTH, STATUS_BAR_HEIGHT};
    SDL_SetRenderDrawColor(sdl_game->renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(sdl_game->renderer, &rect);

    char score[16];
    sprintf(score, "score: %d", sdl_game->game->score);

    SDL_Color color = {0, 0, 0};

    render_text(sdl_game, color, score, 20, 40, 50);

    round_state_t state = sdl_game->game->state;
    if (state == STATE_BLOCKED) {
        render_text(sdl_game, (SDL_Color) {232, 72, 60}, "Prohra", 300, 40, 50);
    } else if (state == STATE_WIN) {
        render_text(sdl_game, (SDL_Color) {60, 232, 69}, "Vyhra", 300, 40, 50);
    }

    if (tiles) {
        game_sdl_render_tiles(sdl_game);
    }
}

void game_sdl_tile_merge_animation_handler(sdl_game_t *sdl_game, animation_t *animation, int progress) {
    vec_tile_t *vec_tile = animation->data;

    int x = vec_tile->vec.x;
    int y = vec_tile->vec.y;

    x = SPACING + (x * TILE_SIZE) + (x * SPACING);
    y = SPACING + (y * TILE_SIZE) + (y * SPACING);

    float increase = (1.0f + (0.2f * ((float) progress / 100.0f))) * TILE_SIZE;
    int offset = (int) ((increase - TILE_SIZE) * 0.5f);

    SDL_Texture *texture = sdl_game->textures[vec_tile->tile != NULL ? vec_tile->tile->index : COLORS_COUNT];
    SDL_Rect rect;
    rect.x = x - offset;
    rect.y = ((int) WINDOW_HEIGHT - y) - TILE_SIZE - offset;
    rect.w = (int) increase;
    rect.h = (int) increase;

    SDL_RenderCopy(sdl_game->renderer, texture, NULL, &rect);
}

void game_sdl_tile_move_animation_handler(sdl_game_t *sdl_game, animation_t *animation, int progress) {
    moved_tile_t *data = animation->data;

    vec2i_t *from = &data->from;
    vec2i_t *to = &data->pos;

    float percent = ((float) progress / 100);
    float x = (float) from->x + ((float) (to->x - from->x) * percent);
    float y = (float) from->y + ((float) (to->y - from->y) * percent);

    x = SPACING + (x * TILE_SIZE) + (x * SPACING);
    y = SPACING + (y * TILE_SIZE) + (y * SPACING);

    SDL_Texture *texture = sdl_game->textures[data->tile != NULL ? data->tile->index : COLORS_COUNT];
    SDL_Rect rect;
    rect.x = (int) x;
    rect.y = ((int) WINDOW_HEIGHT - (int) y) - TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;

    SDL_RenderCopy(sdl_game->renderer, texture, NULL, &rect);
}

SDL_bool game_sdl_run_animations(sdl_game_t *sdl_game, animation_t *animations, Uint8 length) { //dummy handler
    Uint32 ticks = SDL_GetTicks();

    SDL_bool update = SDL_FALSE;
    for (int i = 0; i < length; ++i) {
        animation_t *animation = &animations[i];
        Uint32 running = ticks - animation->start;
        if (running >= animation->duration) {
            if (animation->finishHandler != NULL) {
                animation->finishHandler(sdl_game, animation);
            }
            continue;
        }

        int progress = (int) (animation->fromValue + ((double) (running) / animation->duration) *
                                                     (animation->toValue - animation->fromValue));
        animation->handler(sdl_game, animation, progress);
        update = SDL_TRUE;
    }

    return update;
}

void game_sdl_render_present(sdl_game_t *sdl_game) {
    SDL_SetRenderTarget(sdl_game->renderer, NULL);
    SDL_RenderCopy(sdl_game->renderer, sdl_game->texture, NULL, NULL);
    SDL_RenderPresent(sdl_game->renderer);
    SDL_SetRenderTarget(sdl_game->renderer, sdl_game->texture);
}

void game_sdl_start(sdl_game_t *sdl_game) {
    SDL_Event e;
    SDL_bool quit = SDL_FALSE;
    round_result_t *result = NULL;
    SDL_bool end = SDL_FALSE;


    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = SDL_TRUE;
                break;
            }

            if (e.type == SDL_KEYDOWN) {
                SDL_KeyCode key = e.key.keysym.sym;

                if (isArrowKey(key)) {
                    if (result == NULL) {
                        result = malloc(sizeof(round_result_t));
                    }
                    game_handle_move(sdl_game->game, result, key2dir(key));
                    sdl_game->changed = 1;
                } else if (key == SDLK_ESCAPE) {
                    quit = SDL_TRUE;
                    break;
                }
            }
        }

        if (sdl_game->changed) {
            game_sdl_render(sdl_game, SDL_FALSE);

            if (result != NULL && !end) {
                moved_tile_t *unchanged_tiles[TILES_SIZE], empty_tiles[TILES_SIZE], *moved_tiles[TILES_SIZE];
                Uint8 empty_length = 0, unchanged_length = 0, moved_length = 0;

                for (int i = 0; i < TILES_SIZE; ++i) {
                    moved_tile_t *tile = &result->tile_diffs[i];

                    if (tile->combined != NULL) {
                        tile->tile = tile->combined->tile;
                        moved_tiles[moved_length++] = tile->combined;
                    }

                    if (tile->tile == NULL) {
                        empty_tiles[empty_length++] = *tile;
                    } else if (vec_equals(&tile->from, &tile->pos)) {
                        unchanged_tiles[unchanged_length++] = tile;
                    } else {
                        moved_tiles[moved_length++] = tile;

                        empty_tiles[empty_length++] = *tile;
                        empty_tiles[empty_length - 1].tile = NULL;
                    }
                }

                animation_t animations[moved_length];

                if (moved_length > 0) {
                    for (int i = 0; i < moved_length; ++i) {
                        moved_tile_t *tile = moved_tiles[i];

                        animation_t *anim = &animations[i];

                        anim->data = tile;
                        anim->handler = game_sdl_tile_move_animation_handler;
                        anim->finishHandler = NULL;
                        anim->duration = 100;
                        anim->fromValue = 0;
                        anim->toValue = 100;
                    }
                }

                Uint32 ticks = SDL_GetTicks();
                for (int i = 0; i < moved_length; ++i) {
                    animations[i].start = ticks;
                }

                if (empty_length > 0) {
                    for (int i = 0; i < empty_length; ++i) {
                        moved_tile_t *tile = &empty_tiles[i];

                        game_sdl_render_tile(sdl_game, tile->pos.x, tile->pos.y, tile->tile);
                    }
                }

                while (game_sdl_run_animations(sdl_game, animations, moved_length)) {
                    if (unchanged_length > 0) {
                        for (int i = 0; i < unchanged_length; ++i) {
                            moved_tile_t *tile = unchanged_tiles[i];

                            game_sdl_render_tile(sdl_game, tile->pos.x, tile->pos.y, tile->tile);
                        }
                    }

                    game_sdl_render_present(sdl_game);
                    game_sdl_render(sdl_game, SDL_FALSE);

                    if (empty_length > 0) {
                        for (int i = 0; i < empty_length; ++i) {
                            moved_tile_t *tile = &empty_tiles[i];

                            game_sdl_render_tile(sdl_game, tile->pos.x, tile->pos.y, tile->tile);
                        }
                    }
                }

                for (int i = 0; i < TILES_SIZE; ++i) {
                    if (result->tile_diffs[i].combined != NULL) {
                        free(result->tile_diffs[i].combined);
                        result->tile_diffs[i].combined = NULL;
                    }
                }

                if (result->merged_tiles_length > 0) {
                    animation_t anims[result->merged_tiles_length];

                    for (int i = 0; i < result->merged_tiles_length; ++i) {
                        vec2i_t *pos = &result->merged_tiles[i];

                        animation_t *anim = &anims[i];

                        anim->data = malloc(sizeof(vec_tile_t));
                        ((vec_tile_t *) anim->data)->tile = game_get_tile(sdl_game->game, pos);
                        ((vec_tile_t *) anim->data)->vec = *pos;
                        anim->handler = game_sdl_tile_merge_animation_handler;
                        anim->finishHandler = NULL;
                        anim->duration = 100;
                        anim->fromValue = 0;
                        anim->toValue = 100;
                    }

                    ticks = SDL_GetTicks();
                    for (int i = 0; i < moved_length; ++i) {
                        anims[i].start = ticks;
                    }

                    game_sdl_render(sdl_game, SDL_TRUE);
                    while (game_sdl_run_animations(sdl_game, anims, result->merged_tiles_length)) {
                        game_sdl_render_present(sdl_game);
                        game_sdl_render(sdl_game, SDL_TRUE);
                    }

                    for (int i = 0; i < result->merged_tiles_length; ++i) {
                        free(anims[i].data);
                        anims[i].data = NULL;
                    }
                }

                if (result->state == STATE_WIN || result->state == STATE_BLOCKED) {
                    end = SDL_TRUE;
                }
            }

            game_sdl_render(sdl_game, SDL_TRUE);

            sdl_game->changed = 0;
        }

        game_sdl_render_present(sdl_game);
    }

    if (result != NULL) {
        free(result);
    }
}
