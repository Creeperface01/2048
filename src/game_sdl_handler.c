#include "game_sdl_handler.h"
#include "stdlib.h"
#include "stdio.h"

void resize_texture_by_height(SDL_Rect *rect, SDL_Texture *texture, int max_height) {
    int originHeight;
    int originWidth;

    SDL_QueryTexture(texture, NULL, NULL, &originWidth, &originHeight);

    int height = min(max_height, originHeight);
    int width = (height * originWidth) / originHeight;

    rect->h = height;
    rect->w = width;
}

void create_value_texture(sdl_game_field_texture_t *field_texture, sdl_game_t *sdl_game, int value) {
    SDL_Color color;

    if (value > 4) {
        color = color_from_rgb(COLOR_NUMBER_LIGHT);
    } else {
        color = color_from_rgb(COLOR_NUMBER_DARK);
    }

    char text[10];
    itoa(value, text, 10);

    SDL_Surface *surf = TTF_RenderText_Blended(sdl_game->font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(sdl_game->renderer, surf);

    int maxHeight = TILE_SIZE * 0.8;
    int maxWidth = TILE_SIZE - (TILE_SIZE * 0.2);

    int originHeight;
    int originWidth;

    SDL_QueryTexture(texture, NULL, NULL, &originWidth, &originHeight);

    int height = min(maxHeight, (originHeight * maxWidth) / originWidth);
    int width = (height * originWidth) / originHeight;

    field_texture->texture = texture;
    field_texture->surface = surf;

    field_texture->rect.x = ((TILE_SIZE - width) / 2);
    field_texture->rect.y = ((TILE_SIZE - height) / 2);
    field_texture->rect.w = width;
    field_texture->rect.h = height;
}

sdl_game_t *game_sdl_init(game_t *game) {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

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

    sdl_game->font = TTF_OpenFont("../assets/OpenSans-Bold.ttf", 500);
//    sdl_game->font = TTF_OpenFont("../assets/arial.ttf", 24);

    sdl_game->textures = malloc(sizeof(sdl_game_field_texture_t) * COLORS_COUNT);
    for (unsigned int i = 0; i < COLORS_COUNT; i++) {
        create_value_texture(&sdl_game->textures[i], sdl_game, (int) (1u << (i + 1u)));
    }

    return sdl_game;
}

void game_sdl_destroy(sdl_game_t *sdl_game) {
    TTF_CloseFont(sdl_game->font);
    SDL_DestroyWindow(sdl_game->window);
    SDL_DestroyRenderer(sdl_game->renderer);
    SDL_Quit();

    sdl_game->window = NULL;
    sdl_game->renderer = NULL;
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

void rect_of(SDL_Rect *rect, int x, int y) {
    rect->x = x;
    rect->y = ((int) WINDOW_HEIGHT - y) - TILE_SIZE;

    rect->w = TILE_SIZE;
    rect->h = TILE_SIZE;
}

void game_sdl_render_tile_value(sdl_game_t *sdl_game, int x, int y, int index) {
    sdl_game_field_texture_t *field_texture = &sdl_game->textures[index];
    SDL_Rect rect = field_texture->rect;
    rect.x += x;
    rect.y += y;

    SDL_RenderCopy(sdl_game->renderer, field_texture->texture, NULL, &rect);
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

            if (tile != NULL) {
                game_sdl_render_tile_value(sdl_game, rect.x, rect.y, (int) tile->index);
            }
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

void game_sdl_render(sdl_game_t *sdl_game) {
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

    game_sdl_render_tiles(sdl_game);
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
            game_sdl_render(sdl_game);

//            game_sdl_render_tile_value(sdl_game, 20, 20, 1024);

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
