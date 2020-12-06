#ifndef SRC_UI_TRACK_H_
#define SRC_UI_TRACK_H_

#include <SDL2/SDL.h>
#include "song.h"
#include "ui_pattern.h"

#define UI_TRACK_W 80

typedef struct {
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect target_rect;
    SDL_Texture *pitch_texture[128];
    SDL_Texture *digit_texture[16];
} UiTrack;

UiTrack *ui_track_create(SDL_Renderer *renderer);

void ui_track_destroy(UiTrack *ui_track);

void ui_track_render(UiTrack *ui, Track *track, int pos, int x, int y);


#endif /* SRC_UI_TRACK_H_ */
