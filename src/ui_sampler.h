#ifndef SRC_UI_SAMPLER_H_
#define SRC_UI_SAMPLER_H_

#include <SDL2/SDL.h>
#include "sampler.h"

typedef struct {
    SDL_Renderer *renderer;
} UiSampler;

UiSampler *ui_sampler_create(SDL_Renderer *renderer);

void ui_sampler_destroy(UiSampler *ui_synth);

void ui_sampler_render(UiSampler *ui_synth, Sampler *synth, int x, int y);


#endif /* SRC_UI_SAMPLER_H_ */
