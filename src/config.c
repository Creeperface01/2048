#include "config.h"

void config_init(config_t *cfg, uint8_t rows, uint8_t cols) {
    cfg->rows = rows;
    cfg->cols = cols;
    cfg->tiles_size = rows * cols;
    cfg->max_line_length = rows > cols ? rows : cols;
    cfg->window_height = rows * (TILE_SIZE + SPACING) + SPACING + STATUS_BAR_HEIGHT;
    cfg->window_width = cols * (TILE_SIZE + SPACING) + SPACING;
}

