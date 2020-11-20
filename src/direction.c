#include "direction.h"
#include "stdlib.h"
#include "string.h"

vec2i_t direction_get_offset(vec2i_t *vec, direction_t direction) {
    switch (direction) {
        case DIRECTION_LEFT:
            return vec2i_of(vec->x - 1, vec->y);
        case DIRECTION_RIGHT:
            return vec2i_of(vec->x + 1, vec->y);
        case DIRECTION_UP:
            return vec2i_of(vec->x, vec->y + 1);
        case DIRECTION_DOWN:
        default:
            return vec2i_of(vec->x, vec->y - 1);
    }
}

vec2i_t vec_get_direction(direction_t direction) {
    vec2i_t vec = {0, 0};
    return direction_get_offset(&vec, direction);
}

vec2i_t vec2i_of(int x, int y) {
    return (vec2i_t) {x, y};
}

vec2i_t vec2i_add(vec2i_t *vec1, vec2i_t *vec2) {
    return (vec2i_t) {vec1->x + vec2->x, vec1->y + vec2->y};
}

void vec_cpy(vec2i_t *src, vec2i_t *dest) {
    dest->x = src->x;
    dest->y = src->y;
}

int vec_equals(vec2i_t *vec1, vec2i_t *vec2) {
    return vec1->x == vec2->x && vec1->y == vec2->y;
}

int vec_axis_direction(vec2i_t *vec) {
    axis_t axis = vec_axis(vec);

    switch (axis) {
        case AXIS_X:
            return vec->x / abs(vec->x);
        case AXIS_Y:
            return vec->y / abs(vec->y);
        default:
            return 0;
    }
}

axis_t vec_axis(vec2i_t *vec) {
    return vec->x != 0 && vec->y == 0 ? AXIS_X : (vec->y != 0 && vec->x == 0 ? AXIS_Y : AXIS_NONE);
}

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int find_msb(int a) {
    for (int i = (sizeof(int) * 8) - 1; i >= 0; i--) {
        if ((a & (1 << i)) != 0) { // NOLINT(hicpp-signed-bitwise)
            return i + 1;
        }
    }

    return 0;
}

int bit2byte(int bit_size) {
    return (int) ceil(bit_size / 8.0);
}

void shuffle(void *data, int length, int element_size) {
    char tmp[element_size];

    for (int i = 0; i < length - 1; i++) {
        int j = i + rand() / (RAND_MAX / (length - i) + 1); // NOLINT(cert-msc50-cpp)

        memcpy(tmp, data + j * element_size, element_size);
        memcpy(data + j * element_size, data + i * element_size, element_size);
        memcpy(data + i * element_size, tmp, element_size);
    }
}