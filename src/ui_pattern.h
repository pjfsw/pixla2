#ifndef SRC_UI_PATTERN_H_
#define SRC_UI_PATTERN_H_

#include <SDL2/SDL.h>

#define UI_PATTERN_VISIBLE_NOTES 16
#define UI_PATTERN_EDIT_NOTE_OFFSET 8
#define UI_PATTERN_ROW_SPACING 9

SDL_Color *ui_pattern_get_note_off_color();

SDL_Color *ui_pattern_get_note_color();

SDL_Color *ui_pattern_get_active_row_color();

SDL_Color *ui_pattern_get_beat_highlight_color();


#endif /* SRC_UI_PATTERN_H_ */
