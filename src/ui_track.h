#ifndef SRC_UI_TRACK_H_
#define SRC_UI_TRACK_H_

#include <SDL2/SDL.h>
#include "song.h"
#include "ui_pattern.h"

#define UI_TRACK_W 104
#define UI_TRACK_H (UI_PATTERN_VISIBLE_NOTES * UI_PATTERN_ROW_SPACING)


typedef struct {
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect target_rect;
    SDL_Texture *track_background;
    SDL_Texture *pitch_texture[128];
    SDL_Texture *instrument_texture[10];
    SDL_Texture *no_instrument_texture;
    SDL_Texture *modulation_texture[256];
} UiTrack;

UiTrack *ui_track_create(SDL_Renderer *renderer);

void ui_track_destroy(UiTrack *ui_track);

void ui_track_render(UiTrack *ui, Track *track, int pos, int cursor_pos, bool edit_mode, int x, int y,
    int first_selection, int last_selection);


#endif /* SRC_UI_TRACK_H_ */
