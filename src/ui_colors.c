#include "ui_colors.h"

SDL_Color _color_synth_frame = {
    .r = 8,
    .g = 120,
    .b = 120,
    .a = 255
};

SDL_Color _color_synth_main = {
    .r = 16,
    .g = 240,
    .b = 240,
    .a = 255
};

SDL_Color _color_synth_bg = {
    .r = 3,
    .g = 45,
    .b = 45,
    .a = 255
};

SDL_Color *ui_colors_synth_frame() {
    return &_color_synth_frame;
}

SDL_Color *ui_colors_synth_main() {
    return &_color_synth_main;
}

SDL_Color *ui_colors_synth_bg() {
    return &_color_synth_bg;
}

void ui_colors_set(SDL_Renderer *renderer, SDL_Color *color) {
    if (renderer == NULL || color == NULL) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
}

