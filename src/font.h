#ifndef SRC_FONT_H_
#define SRC_FONT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
    TTF_Font *font;
} Font;

Font font;

bool font_init();

void font_done();

SDL_Texture *font_create_texture(SDL_Renderer *renderer, char *text, SDL_Color *color);

void font_write_scale(SDL_Renderer *renderer, char *text, int x, int y, int scale);

void font_write(SDL_Renderer *renderer, char *text, int x, int y);

#endif /* SRC_FONT_H_ */
