#include "instrument.h"
#include "lookup_tables.h"

void instrument_note_on(Instrument *instrument, int note) {
    if (instrument->type == INSTR_SYNTH) {
        synth_note_on(instrument->synth, note);
    } else if (instrument->type == INSTR_SAMPLER) {
        sampler_note_on(instrument->sampler, note);
    }
}

void instrument_note_off(Instrument *instrument, int note) {
    if (instrument->type == INSTR_SYNTH) {
        synth_note_off(instrument->synth, note);
    } else if (instrument->type == INSTR_SAMPLER) {
        sampler_note_off(instrument->sampler, note);
    }
}

void instrument_off(Instrument *instrument) {
    if (instrument->type == INSTR_SYNTH) {
        synth_off(instrument->synth);
    } else if (instrument->type == INSTR_SAMPLER) {
        sampler_off(instrument->sampler);
    }
}

double instrument_poll(Instrument *instrument, double delta_time) {
    if (instrument->type == INSTR_SYNTH) {
        return synth_poll(instrument->synth, delta_time);
    } else if (instrument->type == INSTR_SAMPLER) {
        return sampler_poll(instrument->sampler, delta_time);
    }
    return 0;
}

double instrument_get_master_level(Instrument *instrument) {
    return lookup_volume(instrument->synth->settings.master_level);
}
