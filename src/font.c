#include "font.h"

bool font_init() {
    memset(&font, 0, sizeof(Font));
    TTF_Init();
    // TODO smarter resource handling
    font.font = TTF_OpenFont("../src/nesfont.fon", 8);
    if (font.font == NULL) {
        fprintf(stderr, "Failed to initialize font: %s\n", TTF_GetError());
        return false;
    }
    return true;
}

void font_done() {
    if (font.font != NULL) {
        TTF_CloseFont(font.font);
    }
}

SDL_Texture *font_create_texture(SDL_Renderer *renderer, char *text, SDL_Color *color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font.font, text, *color);
    if (NULL != surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    } else {
        fprintf(stderr, "Failed to create texture from text: %s\n", TTF_GetError());
    }
    return NULL;
}

void font_write(SDL_Renderer *renderer, char *text, int x, int y) {
    SDL_Color color;
    SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
    SDL_Texture *texture = font_create_texture(renderer, text, &color);
    if (texture != NULL) {
        int w;
        int h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        SDL_Rect rect = {
            .x = x,
            .y = y,
            .w = w,
            .h = h
        };
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }
}
