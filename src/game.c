#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc50-cpp"
#pragma ide diagnostic ignored "cert-msc51-cpp"

#include <stdio.h>
#include "game.h"
#include "stdlib.h"
#include <string.h>
#include "math.h"
#include "time.h"

void init_tiles(game_t *game) {
    game->tile_types = malloc(sizeof(tile_t) * COLORS_COUNT);

    game->tile_types[0] = (tile_t) {0, 2, COLOR_2};
    game->tile_types[1] = (tile_t) {1, 4, COLOR_4};
    game->tile_types[2] = (tile_t) {2, 8, COLOR_8};
    game->tile_types[3] = (tile_t) {3, 16, COLOR_16};
    game->tile_types[4] = (tile_t) {4, 32, COLOR_32};
    game->tile_types[5] = (tile_t) {5, 64, COLOR_64};
    game->tile_types[6] = (tile_t) {6, 128, COLOR_128};
    game->tile_types[7] = (tile_t) {7, 256, COLOR_256};
    game->tile_types[8] = (tile_t) {8, 512, COLOR_512};
    game->tile_types[9] = (tile_t) {9, 1024, COLOR_1024};
    game->tile_types[10] = (tile_t) {10, 2048, COLOR_2048};
}

int game_tile_index(game_t *game, vec2i_t *vec) {
    return (vec->y << game->bit_length) | vec->x; // NOLINT(hicpp-signed-bitwise)
}

game_t *game_create() {
    game_t *game = malloc(sizeof(game_t));

    srand(time(NULL));
    init_tiles(game);

    game->score = 0;
    game->bit_length = max(find_msb(R), find_msb(C));

    int highestIndex = (int) pow(2, (int) game->bit_length * 2);
    game->tiles = malloc(sizeof(tile_t *) * highestIndex);
    game->indices = malloc(sizeof(int) * highestIndex);

    int j = 0;
    for (unsigned int x = 0; x < C; x++) {
        for (unsigned int y = 0; y < R; ++y) {
            game->indices[j++] = (y << game->bit_length) | x;
        }
    }

    game_reset(game);
    return game;
}

void game_reset(game_t *game) {
    game->score = 0;
    game->state = STATE_NONE;

    int highestIndex = (int) pow(2, (int) game->bit_length * 2);
    for (int i = 0; i < highestIndex; i++) {
        game->tiles[i] = NULL;
    }

    vec2i_t vectors[2];
    vectors[0] = vec2i_of(rand() % (int) C, rand() % (int) R);

    do {
        vectors[1] = vec2i_of(rand() % (int) C, rand() % (int) R);
    } while (vec_equals(&vectors[0], &vectors[1]));

    for (int i = 0; i < 2; i++) {
        game_create_tile(game, &vectors[i], 1 << ((rand() % 2) + 1));
    }
}

void game_destroy(game_t *game) {
    free(game->tiles);
    free(game->indices);
    free(game->tile_types);
    free(game);
}

tile_t *game_get_tile(game_t *game, vec2i_t *position) {
    if (!game_check_position(position)) {
        return NULL;
    }

    return game->tiles[game_tile_index(game, position)];
}

int game_set_tile(game_t *game, vec2i_t *position, tile_t *tile) {
    if (!game_check_position(position)) {
        return 0;
    }

    game->tiles[game_tile_index(game, position)] = tile;

    return 1;
}

tile_t *game_create_tile(game_t *game, vec2i_t *position, unsigned int value) {
    if (!game_check_position(position)) {
        return NULL;
    }

    unsigned int i;
    for (i = 0; i < 11; i++) {
        if ((value & (1u << i)) != 0) {
            break;
        }
    }

    if (i >= 11) { //invalid value
        return NULL;
    }

    tile_t *tile = &game->tile_types[i - 1];

    game->tiles[game_tile_index(game, position)] = tile;

    return tile;
}

int find_mergeable(moved_tile_t *tiles, unsigned int length) {
    moved_tile_t *last = NULL;

    for (int i = (int) length - 1; i >= 0; i--) {
        moved_tile_t *tile = &tiles[i];
        if (tile->tile != NULL && last != NULL && last->tile != NULL) {
            if (tile->tile->value == last->tile->value) {
                return (int) i + 1;
            }
        }

        last = tile;
    }

    return -1;
}

int find_empty_tile(game_t *game, vec2i_t *v) {
    unsigned int shuffled[TILES_SIZE];

    for (int i = 0; i < TILES_SIZE; ++i) {
        shuffled[i] = game->indices[i];
    }

    shuffle(shuffled, TILES_SIZE, sizeof(unsigned int));

    for (int i = 0; i < TILES_SIZE; ++i) {
        unsigned int index = shuffled[i];
        tile_t *t = game->tiles[index];

        if (t == NULL) {
            *v = vec2i_of((int) (index & ((unsigned int) pow(2, game->bit_length) - 1)),
                          (int) (index >> game->bit_length));
            return 1;
        }
    }

    return 0;
}

int game_shrink_tiles(moved_tile_t *tiles, int length) {
    int current_offset = 0;
    int changed = 0;
    for (int j = length - 1; j >= 0; j--) {
        if (tiles[j].tile == NULL) {
            current_offset++;
            continue;
        }

        if (current_offset != 0) {
            tiles[j + current_offset].tile = tiles[j].tile;
            vec_cpy(&tiles[j].from, &tiles[j + current_offset].from);

            tiles[j].tile = NULL;
            changed = 1;
        }
    }

    return changed;
}

int game_check_blocked(game_t *game) {
    for (int i = 0; i < TILES_SIZE; ++i) {
        tile_t *tile = game->tiles[game->indices[i]];

        if (tile == NULL) {
            return 0;
        }
    }

    vec2i_t vec;
    tile_t *last;
    tile_t *tile;

    for (int x = 0; x < C; ++x) {
        last = NULL;
        for (int y = 0; y < R; ++y) {
            vec = vec2i_of(x, y);
            tile = game_get_tile(game, &vec);

            if (last != NULL && last->value == tile->value) {
                return 0;
            }

            last = tile;
        }
    }

    for (int y = 0; y < R; ++y) {
        last = NULL;
        for (int x = 0; x < C; ++x) {
            vec = vec2i_of(x, y);
            tile = game_get_tile(game, &vec);

            if (last != NULL && last->value == tile->value) {
                return 0;
            }

            last = tile;
        }
    }

    return 1;
}

void game_handle_move(game_t *game, round_result_t *result, direction_t direction) {
    if (result == NULL) {
        round_result_t r;
        result = &r;
    }
    result->merged_tiles_length = 0;
    result->state = STATE_NONE;

    if (game->state != STATE_NONE) {
        result->state = game->state;
        return;
    }

    vec2i_t offset = vec_get_direction(direction);
    axis_t axis = vec_axis(&offset);
    int maxColumn = axis == AXIS_X ? R : C; // NOLINT(bugprone-branch-clone)
    int tilesLength = axis == AXIS_X ? C : R; // NOLINT(bugprone-branch-clone)

    int changed = 0;

    for (int i = 0; i < maxColumn; ++i) {
        moved_tile_t tiles[tilesLength];

        get_relative_tiles(game, tiles, tilesLength, direction, i);

        changed = game_shrink_tiles(tiles, tilesLength) || changed;

        int mergeableIndex = find_mergeable(tiles, tilesLength);

        if (mergeableIndex >= 0) {
            result->state = STATE_MERGE;
            tiles[mergeableIndex].tile = &game->tile_types[tiles[mergeableIndex].tile->index + 1];
            unsigned int tileValue = tiles[mergeableIndex].tile->value;
            game->score += tileValue;
            if (tileValue == 2048) {
                result->state = game->state = STATE_WIN;
            }

            tiles[mergeableIndex].combined = malloc(sizeof(moved_tile_t));
            memcpy(tiles[mergeableIndex].combined, &tiles[mergeableIndex - 1], sizeof(moved_tile_t));
            tiles[mergeableIndex].combined->pos = tiles[mergeableIndex].pos;

            tiles[mergeableIndex - 1].tile = NULL;
            result->merged_tiles[result->merged_tiles_length++] = tiles[mergeableIndex].pos;
            changed = 1;
        }

        changed = game_shrink_tiles(tiles, tilesLength) || changed;

        for (int j = 0; j < tilesLength; ++j) {
            game_set_tile(game, &tiles[j].pos, tiles[j].tile);
            result->tile_diffs[i * tilesLength + j] = tiles[j];
        }
    }

    if (result->state != STATE_WIN && changed) {
        vec2i_t emptyTile;
        if (find_empty_tile(game, &emptyTile)) {
            result->new_tile = emptyTile;
            game_create_tile(game, &emptyTile, 1 << ((rand() % 2) + 1));
        }
    }

    if (game_check_blocked(game)) {
        result->state = game->state = STATE_BLOCKED;
    }
}

void get_relative_tiles(game_t *game, moved_tile_t *tiles, int length, direction_t direction, int column) {
    vec2i_t offset = vec_get_direction(direction);
    axis_t axis = vec_axis(&offset);
    int axis_direction = vec_axis_direction(&offset);

    int baseX = axis == AXIS_X ? (axis_direction > 0 ? 0 : (int) (C - 1)) : (0 + column);
    int baseY = axis == AXIS_Y ? (axis_direction > 0 ? 0 : (int) (R - 1)) : (0 + column);

    vec2i_t base = {baseX, baseY};

    for (int i = 0; i < length; i++) {
        tile_t *t = game_get_tile(game, &base);
        tiles[i] = (moved_tile_t) {base, base, t, NULL};

        base = vec2i_add(&base, &offset);
    }
}

int game_check_position(vec2i_t *pos) {
    return pos->x >= 0 && pos->x < C && pos->y >= 0 && pos->y < R;
}

#pragma clang diagnostic pop