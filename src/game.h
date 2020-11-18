#ifndef PROJEKT1_GAME_H
#define PROJEKT1_GAME_H

#include "direction.h"
#include "data.h"
#include "config.h"

typedef struct {
    unsigned int score;
    tile_t **tiles;
    unsigned int *indices;
    unsigned int bit_length;
} game_t;

typedef struct {
    vec2i_t merged_tiles[MAX_LINE_LENGTH]; // NOLINT(bugprone-branch-clone)
    int merged_tiles_length;
    int *new_tile;
} round_result_t;

game_t *game_create();

void game_destroy(game_t *game);

round_result_t game_handle_move(game_t *game, direction_t direction);

void get_relative_tiles(game_t *game, vec_tile_t * tiles, int length, direction_t direction, int column);

tile_t *game_create_tile(game_t *game, vec2i_t *position, unsigned int value);

tile_t *game_get_tile(game_t *game, vec2i_t *position);

int game_check_position(vec2i_t *pos);

#endif //PROJEKT1_GAME_H
