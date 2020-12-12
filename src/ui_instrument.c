#include <stdlib.h>

#include "ui_instrument.h"


UiInstrument *ui_instrument_create(SDL_Renderer *renderer) {
    UiInstrument *ui = calloc(1, sizeof(UiInstrument));
    ui->synth = ui_synth_create(renderer);
    if (ui->synth == NULL) {
        ui_instrument_destroy(ui);
        return NULL;
    }
    return ui;
}

void ui_instrument_destroy(UiInstrument *ui) {
    if (ui == NULL) {
        return;
    }

    if (ui->synth != NULL) {
        ui_synth_destroy(ui->synth);
    }

    free(ui);
}

void ui_instrument_render(UiInstrument *ui, Instrument *instrument, int x, int y) {
    if (instrument->type == INSTR_SYNTH) {
        ui_synth_render(ui->synth, instrument->synth, x, y);
    }
}

void ui_instrument_click(UiInstrument *ui, Instrument *instrument, int x, int y) {
    if (instrument->type == INSTR_SYNTH) {
        ui_synth_click(ui->synth, instrument->synth, x, y);
    }
}

void ui_instrument_alter_parameter(UiInstrument *ui, Instrument *instrument, double delta) {
    if (instrument->type == INSTR_SYNTH) {
        ui_synth_alter_parameter(ui->synth, instrument->synth, delta);
    }
}

void ui_instrument_next_parameter(UiInstrument *ui) {
    ui_synth_next_parameter(ui->synth);
}

void ui_instrument_prev_parameter(UiInstrument *ui) {
    ui_synth_prev_parameter(ui->synth);
}


