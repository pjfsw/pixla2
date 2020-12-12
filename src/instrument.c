#include "instrument.h"

void instrument_note_on(Instrument *instrument, int note) {
    if (instrument->type == INSTR_SYNTH) {
        synth_note_on(instrument->synth, note);
    }
}

void instrument_note_off(Instrument *instrument, int note) {
    if (instrument->type == INSTR_SYNTH) {
        synth_note_off(instrument->synth, note);
    }
}

double instrument_poll(Instrument *instrument, double delta_time) {
    if (instrument->type == INSTR_SYNTH) {
        return synth_poll(instrument->synth, delta_time);
    }
    return 0;
}

double instrument_get_master_level(Instrument *instrument) {
    return instrument->synth->master_level;
}
