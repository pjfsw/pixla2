#include "ui_pattern.h"

SDL_Color _ui_pattern_note_modulation_color = {
    .r=191,.g=191,.b=127,.a=255
};

SDL_Color _ui_pattern_note_off_color = {
    .r=91,.g=101,.b=127,.a=255
};
SDL_Color _ui_pattern_note_color = {
    .r=191,.g=201,.b=221,.a=255
};

SDL_Color _ui_pattern_active_row_color= {
    .r=127,.g=127,.b=127,.a=97
};

SDL_Color _ui_pattern_beat_highlight_color= {
    .r=127,.g=127,.b=127,.a=31
};
SDL_Color _ui_pattern_cursor_color = {
    .r=200, .g=100,.b=200,.a=200
};

SDL_Color *ui_pattern_get_note_off_color() {
    return &_ui_pattern_note_off_color;
}

SDL_Color *ui_pattern_get_note_modulation_color() {
    return &_ui_pattern_note_modulation_color;
}

SDL_Color *ui_pattern_get_note_color() {
    return &_ui_pattern_note_color;
}

SDL_Color *ui_pattern_get_active_row_color() {
    return &_ui_pattern_active_row_color;
}

SDL_Color *ui_pattern_get_beat_highlight_color() {
    return &_ui_pattern_beat_highlight_color;
}

SDL_Color *ui_pattern_get_cursor_color() {
    return &_ui_pattern_cursor_color;
}

