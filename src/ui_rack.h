#ifndef SRC_UI_RACK_H_
#define SRC_UI_RACK_H_

#include <SDL2/SDL.h>
#include "ui_instrument.h"
#include "rack.h"

#define RACK_INSTR_W 128

#define UI_RACK_W (UI_INSTR_W + RACK_INSTR_W)
#define UI_RACK_H (UI_INSTR_H)

typedef struct {
    SDL_Texture *synth_textures[NUMBER_OF_INSTRUMENTS];
    SDL_Texture *sampler_textures[NUMBER_OF_INSTRUMENTS];
    SDL_Renderer *renderer;
    UiInstrument *ui_instrument;
    int current_instrument;
} UiRack;

UiRack *ui_rack_create(SDL_Renderer *renderer);

void ui_rack_destroy(UiRack *ui);

void ui_rack_render(UiRack *ui, Rack *rack, int x, int y);

void ui_rack_click(UiRack *ui, Rack *rack, int x, int y);

void ui_rack_alter_parameter(UiRack *ui, Rack *rack, double delta);

void ui_rack_next_parameter(UiRack *ui);

void ui_rack_prev_parameter(UiRack *ui);

void ui_rack_prev_instrument(UiRack *ui);

void ui_rack_next_instrument(UiRack *ui);

#endif /* SRC_UI_RACK_H_ */
