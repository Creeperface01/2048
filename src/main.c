#include <stdio.h>

#define SDL_MAIN_HANDLED

#include "game.h"
#include "game_sdl_handler.h"

int leakTest() {
    int *a = malloc(sizeof(int));

    return 1;
}

int main(int arc, char *argv[]) {
//    int i = leakTest();
//    printf("%d\n", i);
    system("echo %cd%");
    game_t *game = game_create();

//    tile_t *tile = game->tiles[0];

    sdl_game_t *sdl_game = game_sdl_init(game);
    game_sdl_start(sdl_game);

    game_sdl_destroy(sdl_game);
    game_destroy(game);
    return 0;
}
