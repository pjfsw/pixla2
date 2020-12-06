#include "ui_pattern.h"

SDL_Color _ui_pattern_note_off_color = {
    .r=91,.g=31,.b=63,.a=255
};
SDL_Color _ui_pattern_note_color = {
    .r=191,.g=63,.b=127,.a=255
};
SDL_Color _ui_pattern_active_row_color= {
    .r=191,.g=63,.b=127,.a=31
};

SDL_Color *ui_pattern_get_note_off_color() {
    return &_ui_pattern_note_off_color;
}

SDL_Color *ui_pattern_get_note_color() {
    return &_ui_pattern_note_color;
}


SDL_Color *ui_pattern_get_active_row_color() {
    return &_ui_pattern_active_row_color;
}
