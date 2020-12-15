#ifndef PROJEKT1_GAME_H
#define PROJEKT1_GAME_H

#include "direction.h"
#include "data.h"
#include "config.h"
#include <stdint.h>

typedef enum {
    STATE_NONE,
    STATE_MERGE,
    STATE_BLOCKED,
    STATE_WIN
} round_state_t;

typedef struct {
    uint32_t score;
    tile_t *tile_types;
    tile_t **tiles;
    uint32_t *indices;
    uint32_t bit_length;
    round_state_t state;
    config_t config;
} game_t;

typedef struct moved_tile_t {
    vec2i_t from;
    vec2i_t pos;
    tile_t *tile;
    struct moved_tile_t *combined;
} moved_tile_t;

typedef struct {
    vec2i_t *merged_tiles;//[MAX_LINE_LENGTH]; // NOLINT(bugprone-branch-clone)
    int merged_tiles_length;
    moved_tile_t *tile_diffs;//[R * C]; // NOLINT(bugprone-branch-clone)
    vec2i_t new_tile;
    round_state_t state;
} round_result_t;

game_t *game_create(uint8_t width, uint8_t height);

void game_reset(game_t *game);

void game_destroy(game_t *game);

void game_destroy_result(round_result_t *result);

round_result_t *game_handle_move(game_t *game, direction_t direction);

void get_relative_tiles(game_t *game, moved_tile_t *tiles, int length, direction_t direction, int column);

tile_t *game_create_tile(game_t *game, vec2i_t *position, unsigned int value);

tile_t *game_get_tile(game_t *game, vec2i_t *position);

int game_check_position(config_t *cfg, vec2i_t *pos);

#endif //PROJEKT1_GAME_H
