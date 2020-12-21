#include "sdl_game.h"
#include "gfx/SDL2_gfxPrimitives.h"
#include "sdl_assets.h"
#include "sdl_score.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define DATA_KEY "SDL_GAME"

void game_sdl_handle_event(window_handle_t *handle, SDL_Event *e);

void game_sdl_render_main(window_handle_t *handle);

SDL_Texture *create_tile_texture(sdl_game_t *sdl_game, tile_t *tile) {
    window_handle_t *handle = sdl_game->handle;
    float texture_size = TILE_SIZE * MERGE_ANIMATION_SCALE;

    SDL_Texture *texture = SDL_CreateTexture(handle->renderer, SDL_GetWindowPixelFormat(sdl_game->handle->window),
                                             SDL_TEXTUREACCESS_TARGET,
                                             (int) texture_size, (int) texture_size);
    SDL_SetRenderTarget(handle->renderer, texture);

    SDL_SetRenderDrawColorRGB(handle->renderer, COLOR_BACKGROUND, 255);
    SDL_RenderClear(handle->renderer);

    int tmp;
    Uint8 *color_bytes;

    if (tile != NULL) {
        color_bytes = (Uint8 *) &tile->color;
    } else {
        tmp = COLOR_EMPTY;
        color_bytes = (Uint8 *) &tmp;
    }

    roundedBoxRGBA(handle->renderer, 0, 0, texture_size - 1, texture_size - 1, 5, color_bytes[2],
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

        SDL_Surface *surf = TTF_RenderText_Blended(sdl_main_get_font(sdl_game->handle->data), text, color);
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(handle->renderer, surf);

        int maxHeight = (int) (texture_size * 0.8f);
        int maxWidth = (int) (texture_size - (texture_size * 0.2f));

        int originHeight;
        int originWidth;

        SDL_QueryTexture(text_texture, NULL, NULL, &originWidth, &originHeight);

        int height = min(maxHeight, (originHeight * maxWidth) / originWidth);
        int width = (height * originWidth) / originHeight;

        SDL_Rect rect = {(int) ((texture_size - (float) width) / 2), (int) ((texture_size - (float) height) / 2), width,
                         height};
        SDL_RenderCopy(handle->renderer, text_texture, NULL, &rect);

        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(surf);
    }

    return texture;
}

window_handle_t *game_sdl_init(sdl_data_t *data, game_t *game) {
    sdl_game_t *sdl_game = malloc(sizeof(sdl_game_t));
    config_t *cfg = &game->config;

    window_handle_t *handle = create_window(WINDOW_GAME, data,
                                            "2048",
                                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            cfg->window_width, cfg->window_height,
                                            SDL_WINDOW_SHOWN);
    SDL_SetWindowData(handle->window, DATA_KEY, sdl_game);

    handle->closeHandler = game_sdl_destroy;
    handle->eventHandler = game_sdl_handle_event;
    handle->renderHandler = game_sdl_render_main;

    sdl_game->handle = handle;
    sdl_game->changed = true;
    sdl_game->result = NULL;
    sdl_game->end = false;

    sdl_game->game = game;

    sdl_game->texture = SDL_CreateTexture(handle->renderer, SDL_GetWindowPixelFormat(handle->window),
                                          SDL_TEXTUREACCESS_TARGET,
                                          cfg->window_width, cfg->window_height);
    SDL_SetRenderTarget(handle->renderer, sdl_game->texture);

    sdl_game->textures = malloc(sizeof(SDL_Texture *) * (COLORS_COUNT + 1));

    for (unsigned int i = 0; i < COLORS_COUNT; i++) {
        sdl_game->textures[i] = create_tile_texture(sdl_game, &sdl_game->game->tile_types[i]);
    }

    sdl_game->textures[COLORS_COUNT] = create_tile_texture(sdl_game, NULL);

    SDL_SetRenderTarget(handle->renderer, sdl_game->texture);

    return handle;
}

void game_sdl_destroy(window_handle_t *handle) {
    sdl_game_t *sdl_game = SDL_GetWindowData(handle->window, DATA_KEY);

    SDL_DestroyTexture(sdl_game->texture);
    game_destroy(sdl_game->game);

    for (int i = 0; i < COLORS_COUNT; ++i) {
        SDL_DestroyTexture(sdl_game->textures[i]);
        sdl_game->textures[i] = NULL;
    }

    free(sdl_game->textures);

    if (sdl_game->result != NULL) {
        game_destroy_result(sdl_game->result);
    }

    sdl_game->handle = NULL;
    sdl_game->texture = NULL;
    sdl_game->game = NULL;
    sdl_game->result = NULL;
    sdl_game->textures = NULL;

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
    rect.y = ((int) sdl_game->game->config.window_height - y) - TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;

    SDL_RenderCopy(sdl_game->handle->renderer, texture, NULL, &rect);
}

void game_sdl_render_tiles(sdl_game_t *sdl_game) {
    config_t *cfg = &sdl_game->game->config;
    vec2i_t vec;

    for (int x = 0; x < cfg->cols; ++x) {
        for (int y = 0; y < cfg->rows; ++y) {
            vec.x = x;
            vec.y = y;
            game_sdl_render_tile(sdl_game, x, y, game_get_tile(sdl_game->game, &vec));
        }
    }
}

void game_sdl_render(sdl_game_t *sdl_game, bool tiles) {
    window_handle_t *handle = sdl_game->handle;

    SDL_SetRenderDrawColorRGB(handle->renderer, COLOR_BACKGROUND, 255);
    SDL_RenderClear(handle->renderer);

    SDL_Rect rect = {0, 0, sdl_game->game->config.window_width, STATUS_BAR_HEIGHT};
    SDL_SetRenderDrawColor(handle->renderer, 150, 150, 150, 255);
    SDL_RenderFillRect(handle->renderer, &rect);

    char score[16];
    sprintf(score, "score: %d", sdl_game->game->score);

    SDL_Color color = {0, 0, 0, 255};

    render_text(sdl_game->handle, color, score, 20, 40, 50, false);

    round_state_t state = sdl_game->game->state;
    if (state == STATE_BLOCKED) {
        render_text(sdl_game->handle, (SDL_Color) {232, 72, 60, 255}, "Prohra", 300, 40, 50, false);
    } else if (state == STATE_WIN) {
        render_text(sdl_game->handle, (SDL_Color) {60, 232, 69, 255}, "Vyhra", 300, 40, 50, false);
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
    rect.y = ((int) sdl_game->game->config.window_height - y) - TILE_SIZE - offset;
    rect.w = (int) increase;
    rect.h = (int) increase;

    SDL_RenderCopy(sdl_game->handle->renderer, texture, NULL, &rect);
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
    rect.y = ((int) sdl_game->game->config.window_height - (int) y) - TILE_SIZE;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;

    SDL_RenderCopy(sdl_game->handle->renderer, texture, NULL, &rect);
}

bool game_sdl_run_animations(sdl_game_t *sdl_game, animation_t *animations, Uint8 length) { //dummy handler
    Uint32 ticks = SDL_GetTicks();

    bool update = false;
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
        update = true;
    }

    return update;
}

void game_sdl_render_present(sdl_game_t *sdl_game) {
    SDL_Renderer *renderer = sdl_game->handle->renderer;

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, sdl_game->texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_SetRenderTarget(renderer, sdl_game->texture);
}

void game_sdl_handle_event(window_handle_t *handle, SDL_Event *e) {
    if (e->type == SDL_KEYDOWN) {
        sdl_game_t *sdl_game = SDL_GetWindowData(handle->window, DATA_KEY);

        if (sdl_game->end || sdl_game->changed) {
            return;
        }

        SDL_KeyCode key = e->key.keysym.sym;

        if (isArrowKey(key)) {
            if (sdl_game->result != NULL) {
                game_destroy_result(sdl_game->result);
            }

            sdl_game->result = game_handle_move(sdl_game->game, key2dir(key));
            sdl_game->changed = true;
        }
    }
}

void game_sdl_render_main(window_handle_t *handle) {
    sdl_game_t *sdl_game = SDL_GetWindowData(handle->window, DATA_KEY);

    if (sdl_game->changed) {
        game_sdl_render(sdl_game, false);
        round_result_t *result = sdl_game->result;

        if (result != NULL && !sdl_game->end) {
            config_t *cfg = &sdl_game->game->config;
            uint16_t tiles_size = cfg->tiles_size;

            moved_tile_t *unchanged_tiles[tiles_size], empty_tiles[tiles_size], *moved_tiles[tiles_size];
            Uint8 empty_length = 0, unchanged_length = 0, moved_length = 0;

            for (int i = 0; i < tiles_size; ++i) {
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
                game_sdl_render(sdl_game, false);

                if (empty_length > 0) {
                    for (int i = 0; i < empty_length; ++i) {
                        moved_tile_t *tile = &empty_tiles[i];

                        game_sdl_render_tile(sdl_game, tile->pos.x, tile->pos.y, tile->tile);
                    }
                }
            }

            for (int i = 0; i < tiles_size; ++i) {
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
                for (int i = 0; i < result->merged_tiles_length; ++i) {
                    anims[i].start = ticks;
                }

                game_sdl_render(sdl_game, true);
                while (game_sdl_run_animations(sdl_game, anims, result->merged_tiles_length)) {
                    game_sdl_render_present(sdl_game);
                    game_sdl_render(sdl_game, true);
                }

                for (int i = 0; i < result->merged_tiles_length; ++i) {
                    free(anims[i].data);
                    anims[i].data = NULL;
                }
            }

            if (result->state == STATE_WIN || result->state == STATE_BLOCKED) {
                sdl_game->end = true;
                sdl_score_add(sdl_game->game->score);
            }
        }

        game_sdl_render(sdl_game, true);

        sdl_game->changed = false;
    }

    game_sdl_render_present(sdl_game);
}
