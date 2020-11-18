#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc50-cpp"
#pragma ide diagnostic ignored "cert-msc51-cpp"

#include <stdio.h>
#include "game.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"

tile_t *g_tiles = NULL;

void init_tiles() {
    g_tiles = malloc(sizeof(tile_t) * COLORS_COUNT);

    g_tiles[0] = (tile_t) {0, 2, COLOR_2};
    g_tiles[1] = (tile_t) {1, 4, COLOR_4};
    g_tiles[2] = (tile_t) {2, 8, COLOR_8};
    g_tiles[3] = (tile_t) {3, 16, COLOR_16};
    g_tiles[4] = (tile_t) {4, 32, COLOR_32};
    g_tiles[5] = (tile_t) {5, 64, COLOR_64};
    g_tiles[6] = (tile_t) {6, 128, COLOR_128};
    g_tiles[7] = (tile_t) {7, 256, COLOR_256};
    g_tiles[8] = (tile_t) {8, 512, COLOR_512};
    g_tiles[9] = (tile_t) {9, 1024, COLOR_1024};
    g_tiles[10] = (tile_t) {10, 2048, COLOR_2048};
}

int game_tile_index(game_t *game, vec2i_t *vec) {
    return (vec->y << game->bit_length) | vec->x; // NOLINT(hicpp-signed-bitwise)
}

game_t *game_create() {
    if (g_tiles == NULL) {
        srand(time(NULL));
        init_tiles();
    }

    game_t *game = malloc(sizeof(game_t));

    game->score = 0;
    game->bit_length = max(find_msb(R), find_msb(C));

    int highestIndex = (int) pow(2, (int) game->bit_length * 2);
    game->tiles = malloc(sizeof(tile_t *) * highestIndex);
    game->indices = malloc(sizeof(int) * highestIndex);

    for (int i = 0; i < highestIndex; i++) {
        game->tiles[i] = NULL;
    }

    int j = 0;
    for (unsigned int x = 0; x < C; x++) {
        for (unsigned int y = 0; y < R; ++y) {
            game->indices[j++] = (y << game->bit_length) | x;
        }
    }

    vec2i_t vectors[2];
    vectors[0] = vec2i_of(rand() % (int) C, rand() % (int) R);

    do {
        vectors[1] = vec2i_of(rand() % (int) C, rand() % (int) R);
    } while (vec_equals(&vectors[0], &vectors[1]));

    for (int i = 0; i < 2; i++) {
        game_create_tile(game, &vectors[i], 1 << ((rand() % 2) + 1));
    }

    return game;
}

void game_destroy(game_t *game) {
    free(game->tiles);
    free(game->indices);
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

    tile_t *tile = &g_tiles[i - 1];

    game->tiles[game_tile_index(game, position)] = tile;

    return tile;
}

int find_mergeable(vec_tile_t *tiles, unsigned int length) {
    vec_tile_t *last = NULL;

    for (int i = (int) length - 1; i >= 0; i--) {
        vec_tile_t *tile = &tiles[i];
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

int game_shrink_tiles(vec_tile_t *tiles, int length) {
    int filled = 0;
    int current_offset = 0;
    for (int j = length - 1; j >= 0; j--) {
        if (tiles[j].tile == NULL) {
            current_offset++;
            continue;
        }

        if (current_offset != 0) {
            tiles[j + current_offset].tile = tiles[j].tile;
            tiles[j].tile = NULL;
        }
        filled++;
    }

    return filled;
}

round_result_t game_handle_move(game_t *game, direction_t direction) {
    vec2i_t offset = vec_get_direction(direction);
    axis_t axis = vec_axis(&offset);
    int maxColumn = axis == AXIS_X ? R : C; // NOLINT(bugprone-branch-clone)
    int tilesLength = axis == AXIS_X ? C : R; // NOLINT(bugprone-branch-clone)

    round_result_t result;
    result.merged_tiles_length = 0;

    for (int i = 0; i < maxColumn; ++i) {
        vec_tile_t tiles[tilesLength];

        get_relative_tiles(game, tiles, tilesLength, direction, i);

        game_shrink_tiles(tiles, tilesLength);

        int mergeableIndex = find_mergeable(tiles, tilesLength);

        if (mergeableIndex >= 0) {
            tiles[mergeableIndex].tile = &g_tiles[tiles[mergeableIndex].tile->index + 1];

            tiles[mergeableIndex - 1].tile = NULL;
            result.merged_tiles[result.merged_tiles_length++] = tiles[mergeableIndex].vec;
        }

        game_shrink_tiles(tiles, tilesLength);

        for (int j = 0; j < tilesLength; ++j) {
            game_set_tile(game, &tiles[j].vec, tiles[j].tile);
        }
    }

    vec2i_t emptyTile;
    if (find_empty_tile(game, &emptyTile)) {
        game_create_tile(game, &emptyTile, 1 << ((rand() % 2) + 1));
    }

    return result;
}

void get_relative_tiles(game_t *game, vec_tile_t *tiles, int length, direction_t direction, int column) {
    vec2i_t offset = vec_get_direction(direction);
    axis_t axis = vec_axis(&offset);
    int axis_direction = vec_axis_direction(&offset);

    int baseX = axis == AXIS_X ? (axis_direction > 0 ? 0 : (int) (C - 1)) : (0 + column);
    int baseY = axis == AXIS_Y ? (axis_direction > 0 ? 0 : (int) (R - 1)) : (0 + column);

    vec2i_t base = {baseX, baseY};

//    int length = axis == AXIS_X ? C : R; // NOLINT(bugprone-branch-clone)
//    vec_tile_t *tiles = malloc(sizeof(vec_tile_t) * length);

    for (int i = 0; i < length; i++) {
        tile_t *t = game_get_tile(game, &base);
        tiles[i] = (vec_tile_t) {base, t};

        base = vec2i_add(&base, &offset);
    }

//    return tiles;
}

int game_check_position(vec2i_t *pos) {
    return pos->x >= 0 && pos->x < C && pos->y >= 0 && pos->y < R;
}

#pragma clang diagnostic pop