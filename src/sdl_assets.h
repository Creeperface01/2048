#ifndef INC_2048_SDL_ASSETS_H
#define INC_2048_SDL_ASSETS_H

#include "stdlib.h"
#include "stdio.h"
#include <string.h>

#if defined ( WIN32 )

#include "../libs-win/base64/base64.h"

#else
#include "../libs-unix/base64/base64.h"
#endif

#include "SDL.h"
#include "SDL_ttf.h"

TTF_Font *load_font(int size);

#endif //INC_2048_SDL_ASSETS_H
