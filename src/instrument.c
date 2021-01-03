#include "instrument.h"
#include "lookup_tables.h"

int instrument_note_on(Instrument *instrument, int note, int velocity) {
    if (instrument->type == INSTR_SYNTH) {
        return synth_note_on(instrument->synth, note, velocity);
    } else if (instrument->type == INSTR_SAMPLER) {
        return sampler_note_on(instrument->sampler, note, velocity);
    }
    return 0;
}

void instrument_note_off(Instrument *instrument, int voice_id) {
    if (instrument->type == INSTR_SYNTH) {
        synth_note_off(instrument->synth, voice_id);
    } else if (instrument->type == INSTR_SAMPLER) {
        sampler_note_off(instrument->sampler, voice_id);
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
