#ifndef SRC_UI_TRACKPOS_H_
#define SRC_UI_TRACKPOS_H_

#include "ui_pattern.h"
#include "song.h"

#define UI_TRACKPOS_W 16
#define UI_TRACKPOS_H (UI_PATTERN_VISIBLE_NOTES * UI_PATTERN_ROW_SPACING)

typedef struct {
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect target_rect;
    SDL_Rect source_rect;
} UiTrackPos;

UiTrackPos *ui_trackpos_create(SDL_Renderer *renderer);

void ui_trackpos_destroy(UiTrackPos *ui);

void ui_trackpos_render(UiTrackPos *ui, int pos, int x, int y);


#endif /* SRC_UI_TRACKPOS_H_ */
