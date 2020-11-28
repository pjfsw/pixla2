#ifndef SRC_UI_SYNTH_H_
#define SRC_UI_SYNTH_H_

#include <SDL2/SDL.h>
#include "synth.h"


#define UI_SYNTH_W 512
#define UI_SYNTH_H 400

typedef double* (*ParameterFunc)(Synth *synth);

typedef struct {
    int x;
    int y;
    int w;
    int h;
    ParameterFunc parameter_func;
    SDL_Texture *texture;
} ParameterController;

typedef struct {
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Rect target_rect;
    ParameterController *parameter_controllers;
    int number_of_parameter_controllers;
    int current_parameter;
} UiSynth;


UiSynth *ui_synth_create(SDL_Renderer *renderer);

void ui_synth_destroy(UiSynth *ui_synth);

void ui_synth_render(UiSynth *ui_synth, Synth *synth, int x, int y);

void ui_synth_click(UiSynth *ui, Synth *synth, int x, int y);

void ui_synth_alter_parameter(UiSynth *ui, Synth *synth, double delta);

void ui_synth_next_parameter(UiSynth *ui);

void ui_synth_prev_parameter(UiSynth *ui);

#endif /* SRC_UI_SYNTH_H_ */
