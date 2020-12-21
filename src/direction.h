#ifndef PROJEKT1_DIRECTION_UTILS_H
#define PROJEKT1_DIRECTION_UTILS_H

#include "data.h"
#include "config.h"
#include <stdlib.h>
#include <math.h>


vec2i_t vec2i_of(int x, int y);

vec2i_t vec2i_add(vec2i_t *vec1, vec2i_t *vec2);

//unsigned int position_from(unsigned int x, unsigned int y) {
//    x &= 0xffu;
//    y &= 0xffu;
//    return (y << 8u) | x;
//}

vec2i_t direction_get_offset(vec2i_t *vec, direction_t direction);

vec2i_t vec_get_direction(direction_t direction);

int vec_equals(vec2i_t *vec1, vec2i_t *vec2);

void vec_cpy(vec2i_t *src, vec2i_t *dest);

axis_t vec_axis(vec2i_t *vec);

int vec_axis_direction(vec2i_t *vec);

int max(int a, int b);

int min(int a, int b);

int find_msb(int a);

int bit2byte(int bit_size);

void shuffle(void *data, int length, int element_size);

void array_fill(void **src, void *value, uint32_t length);

#endif //PROJEKT1_DIRECTION_UTILS_H
