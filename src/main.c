#define SDL_MAIN_HANDLED

#include "game.h"
#include "sdl_game.h"
#include "string.h"
#include "stdbool.h"
#include <stdio.h>
#include <errno.h>

typedef struct {
    char *name;
    char *value;
} arg_t;

int scan_args(int size, char **input, arg_t **args) {
    *args = malloc(size * sizeof(arg_t *));

    int length = 0;

    bool expectValue = false;
    char name[32];

    for (int i = 1; i < size; ++i) {
        char *p = input[i];
        arg_t *arg = NULL;

        if (p[0] == '-') {
            if (strlen(p) == 1) {
                printf("Invalid option name '%s'\n", p);
                return -1;
            }

            if (expectValue) {
                arg = malloc(sizeof(arg_t));

                uint8_t nameLength = strlen(name);
                arg->name = malloc(sizeof(char) * (nameLength + 1));
                strcpy(arg->name, name);
                arg->value = NULL;

                expectValue = false;
            } else {
                strcpy(name, p + 1);
                expectValue = true;
            }
        } else if (!expectValue) {
            printf("Expected param name\n");
            return -1;
        } else {
            arg = malloc(sizeof(arg_t));

            uint8_t nameLength = strlen(name);
            arg->name = malloc(sizeof(char) * (nameLength + 1));
            strcpy(arg->name, name);

            uint8_t valLength = strlen(p);
            arg->value = malloc(sizeof(char) * (valLength + 1));
            strcpy(arg->value, p);

            expectValue = false;
        }

        if (arg != NULL) {
            memcpy(&(*args)[length++], arg, sizeof(arg_t));
        }
    }

    return length;
}

arg_t *get_arg(arg_t *args, uint8_t size, char *name) {
    for (int i = 0; i < size; ++i) {
        arg_t *arg = &args[i];

        if (strcasecmp(arg->name, name) == 0) {
            return arg;
        }
    }

    return NULL;
}

bool get_arg_bool(arg_t *args, uint8_t size, char *name) {
    return get_arg(args, size, name) != NULL;
}

bool parse_int(char *str, int *v) {
    errno = 0;
    char *endptr = NULL;

    *v = (int) strtol(str, &endptr, 10);

    if (endptr == str || errno != 0) {
        printf("Please provide a valid number\n");
        return false;
    }

    return true;
}

int main(int arc, char *argv[]) {
    arg_t *args;
    int args_length = scan_args(arc, argv, &args);

    if (args_length < 0) {
        return 1;
    }

    arg_t *width = get_arg(args, args_length, "C");
    arg_t *height = get_arg(args, args_length, "R");
    bool reset = get_arg_bool(args, args_length, "s");

    if (reset) {
        remove("scores.bin");
    }

    if (width == NULL || height == NULL) {
        printf("Arguments R and C are required\n");
        free(args);
        return 1;
    }

    int w, h;

    if (!parse_int(width->value, &w) || !parse_int(height->value, &h)) {
        return 1;
    }

    if (w < 2 || w > 100 || h < 2 || h > 100) {
        printf("Plane size must be between 2 and 100\n");
    }

    sdl_main_init(w, h);
    return 0;
}
