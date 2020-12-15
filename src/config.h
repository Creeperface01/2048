#ifndef PROJEKT1_CONFIG_H
#define PROJEKT1_CONFIG_H

#include "stdint.h"
#include "string.h"

//#define R 8u
//#define C 8u

//#define TILES_SIZE (R * C)
//#define MAX_LINE_LENGTH (R > C ? R : C)

#define SPACING 15u
#define TILE_SIZE 100

#define STATUS_BAR_HEIGHT 100

//#define WINDOW_HEIGHT (R * (TILE_SIZE + SPACING) + SPACING + STATUS_BAR_HEIGHT)
//#define WINDOW_WIDTH (C * (TILE_SIZE + SPACING) + SPACING)

#define MERGE_ANIMATION_SCALE 1.5f

#define COLOR_NUMBER_DARK 0x776E65
#define COLOR_NUMBER_LIGHT 0xF9F6F2

#define COLOR_EMPTY 0xCDC1B5
#define COLOR_BACKGROUND 0xBAAE9F

#define COLOR_2 0xEEE4DA
#define COLOR_4 0xEDE0C8
#define COLOR_8 0xF2B179
#define COLOR_16 0xF59563
#define COLOR_32 0xF67C60
#define COLOR_64 0xF65E3B
#define COLOR_128 0xEDCF73
#define COLOR_256 0xEDCC62
#define COLOR_512 0xEDC850
#define COLOR_1024 0xEDC537
#define COLOR_2048 0xEDC22D

#define COLORS_COUNT 11

#if defined ( WIN32 )
#define strcmpicase(s0, s1) strcmpi(s0, s1)
#else
#define strcmpicase(s0, s1) strcasecmp(s0, s1)
#endif

typedef struct {
    uint8_t rows;
    uint8_t cols;
    uint16_t tiles_size;
    uint16_t max_line_length;
    uint16_t window_height;
    uint16_t window_width;
} config_t;

void config_init(config_t *cfg, uint8_t rows, uint8_t cols);

#endif //PROJEKT1_CONFIG_H
