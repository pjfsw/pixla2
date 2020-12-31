#ifndef SRC_UI_RACK_H_
#define SRC_UI_RACK_H_

#include <SDL2/SDL.h>
#include "ui_instrument.h"
#include "ui_mixer.h"
#include "rack.h"
#include "ui_boundary.h"

#define UI_RACK_W SCRW
#define UI_RACK_H UI_INSTRUMENT_H
#define RACK_INSTR_W (UI_RACK_W - UI_INSTRUMENT_W)

typedef enum {
    UI_RACK_NONE,
    UI_RACK_INSTRUMENT,
    UI_RACK_MIXER
} UiRackMode;

typedef struct {
    SDL_Texture *synth_textures[NUMBER_OF_INSTRUMENTS];
    SDL_Texture *sampler_textures[NUMBER_OF_INSTRUMENTS];
    SDL_Renderer *renderer;
    UiInstrument *ui_instrument;
    UiMixer *ui_mixer;
    UiRackMode mode;
    Uint8 current_instrument;
} UiRack;

UiRack *ui_rack_create(SDL_Renderer *renderer);

void ui_rack_set_mode(UiRack *ui, UiRackMode mode);

void ui_rack_destroy(UiRack *ui);

void ui_rack_render(UiRack *ui, Rack *rack, int x, int y);

void ui_rack_click(UiRack *ui, Rack *rack, int x, int y);

void ui_rack_alter_parameter(UiRack *ui, Rack *rack, double delta);

void ui_rack_next_parameter(UiRack *ui);

void ui_rack_prev_parameter(UiRack *ui);

void ui_rack_prev_instrument(UiRack *ui);

void ui_rack_next_instrument(UiRack *ui);

#endif /* SRC_UI_RACK_H_ */
