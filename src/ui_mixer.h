#ifndef SRC_UI_MIXER_H_
#define SRC_UI_MIXER_H_

#include <SDL2/SDL.h>

#include "mixer.h"
#include "ui_component_manager.h"
#include "ui_boundary.h"

#define UI_MIXER_W UI_INSTRUMENT_W
#define UI_MIXER_H UI_INSTRUMENT_H

typedef struct {
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    UiComponentManager *cmgr;
} UiMixer;

UiMixer *ui_mixer_create(SDL_Renderer *renderer);

void ui_mixer_destroy(UiMixer *ui);

void ui_mixer_render(UiMixer *ui, Mixer *mixer, int x, int y);

void ui_mixer_click(UiMixer *ui, Mixer *mixer, int x, int y);

#endif /* SRC_UI_MIXER_H_ */
