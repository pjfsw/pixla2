#ifndef SRC_UI_INSTRUMENT_H_
#define SRC_UI_INSTRUMENT_H_

#include "ui_synth.h"
#include "instrument.h"

#define UI_INSTR_W UI_SYNTH_W
#define UI_INSTR_H UI_SYNTH_H

typedef struct {
    UiSynth *synth;
} UiInstrument;

UiInstrument *ui_instrument_create(SDL_Renderer *renderer);

void ui_instrument_destroy(UiInstrument *ui);

void ui_instrument_render(UiInstrument *ui, Instrument *instrument, int x, int y);

void ui_instrument_click(UiInstrument *ui, Instrument *instrument, int x, int y);

void ui_instrument_alter_parameter(UiInstrument *ui, Instrument *instrument, double delta);

void ui_instrument_next_parameter(UiInstrument *ui);

void ui_instrument_prev_parameter(UiInstrument *ui);

#endif /* SRC_UI_INSTRUMENT_H_ */
