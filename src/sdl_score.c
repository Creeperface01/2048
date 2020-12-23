#include "sdl_score.h"
#include <stdlib.h>
#include <stdio.h>

uint16_t load_scores(uint16_t **data) {
    FILE *f = fopen("scores.bin", "rb");

    if (f == NULL) {
        return 0;
    }

    uint16_t length;
    fread(&length, sizeof(uint16_t), 1, f);

    *data = malloc(sizeof(uint16_t) * length);
    fread(*data, sizeof(uint16_t), length, f);

    fclose(f);

    return length;
}

void save_scores(uint16_t *data, uint16_t length) {
    if (length == 0) {
        return;
    }

    FILE *f = fopen("scores.bin", "wb");

    fwrite(&length, sizeof(uint16_t), 1, f);
    fwrite(data, sizeof(uint16_t), length, f);

    fclose(f);
}

void sdl_score_add(uint16_t score) {
    uint16_t *scores = NULL;
    uint16_t length = load_scores(&scores);

    for (int i = 0; i < length; ++i) {
        if (score < scores[i]) {
            return;
        }
    }

    if (length == 0) {
        scores = malloc(sizeof(uint16_t));
    } else {
        scores = realloc(scores, sizeof(uint16_t) * (length + 1));
    }

    scores[length] = score;

    save_scores(scores, length + 1);
    free(scores);
}

window_handle_t *sdl_score_create_window(sdl_data_t *data) {
    window_handle_t *handle = create_window(WINDOW_SCORES, data,
                                            "High scores",
                                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            300, 600,
                                            SDL_WINDOW_SHOWN);

    uint16_t *scores;
    uint16_t length = load_scores(&scores);

    if (length > 0) {
        uint16_t y = 10;
        for (int i = 0; i < length; ++i) {
            char str[32];
            sprintf(str, "%d. %u", i, scores[length - (i + 1)]);

            render_text(handle, COLOR_BLACK, str, 10, y, 20, false);

            y += 30;
        }

        free(scores);
    }

    SDL_RenderPresent(handle->renderer);
    return handle;
}