#ifndef PROJEKT1_DATA_H
#define PROJEKT1_DATA_H

typedef enum {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} direction_t;

typedef enum {
    AXIS_X,
    AXIS_Y,
    AXIS_NONE
} axis_t;

typedef struct {
    int x;
    int y;
} vec2i_t;

typedef struct {
    unsigned int index;
    unsigned int value;
    unsigned int color;
} tile_t;

typedef struct {
    vec2i_t vec;
    tile_t *tile;
} vec_tile_t;

#endif //PROJEKT1_DATA_H
