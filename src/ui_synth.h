#ifndef SRC_UI_SYNTH_H_
#define SRC_UI_SYNTH_H_

#include <SDL2/SDL.h>
#include "synth.h"
#include "ui_component_manager.h"

#define UI_SYNTH_W 640
#define UI_SYNTH_H 464

typedef struct {
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect target_rect;
    UiComponentManager *cmgr;
    Vca visual_vca;
} UiSynth;


UiSynth *ui_synth_create(SDL_Renderer *renderer);

void ui_synth_destroy(UiSynth *ui_synth);

void ui_synth_render(UiSynth *ui_synth, Synth *synth, int x, int y);

void ui_synth_click(UiSynth *ui, Synth *synth, int x, int y);

void ui_synth_alter_parameter(UiSynth *ui, Synth *synth, double delta);

void ui_synth_next_parameter(UiSynth *ui);

void ui_synth_prev_parameter(UiSynth *ui);

#endif /* SRC_UI_SYNTH_H_ */
