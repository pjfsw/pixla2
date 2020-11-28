#ifndef SRC_UI_SYNTH_H_
#define SRC_UI_SYNTH_H_

#include <SDL2/SDL.h>
#include "synth.h"


#define UI_SYNTH_W 512
#define UI_SYNTH_H 400

typedef struct {
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Texture *slider_texture;
    SDL_Rect target_rect;
} UiSynth;


UiSynth *ui_synth_create(SDL_Renderer *renderer);

void ui_synth_destroy(UiSynth *ui_synth);

void ui_synth_render(UiSynth *ui_synth, Synth *synth, int x, int y);

#endif /* SRC_UI_SYNTH_H_ */
