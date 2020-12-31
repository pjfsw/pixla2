#include "ui_colors.h"

SDL_Color _color_synth_frame = {
    .r = 30,
    .g = 150,
    .b = 160,
    .a = 255
};


SDL_Color _color_synth_main = {
    .r = 90,
    .g = 200,
    .b = 210,
    .a = 255
};

SDL_Color _color_synth_highlight = {
    .r = 205,
    .g = 245,
    .b = 255,
    .a = 255
};

SDL_Color _color_synth_bg = {
    .r = 10,
    .g = 60,
    .b = 70,
    .a = 255
};

SDL_Color _ui_colors_sequencer_status = {
    .r = 127,
    .g = 127,
    .b = 127,
    .a = 255
};

SDL_Color _ui_colors_sequencer_highlight = {
    .r = 255,
    .g = 255,
    .b = 255,
    .a = 255
};


SDL_Color *ui_colors_synth_frame() {
    return &_color_synth_frame;
}

SDL_Color *ui_colors_synth_main() {
    return &_color_synth_main;
}

SDL_Color *ui_colors_synth_highlight() {
    return &_color_synth_highlight;
}

SDL_Color *ui_colors_synth_bg() {
    return &_color_synth_bg;
}

SDL_Color *ui_colors_sequencer_status() {
    return &_ui_colors_sequencer_status;
}

SDL_Color *ui_colors_sequencer_highlight() {
    return &_ui_colors_sequencer_highlight;
}

void ui_colors_set(SDL_Renderer *renderer, SDL_Color *color) {
    if (renderer == NULL || color == NULL) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
}

