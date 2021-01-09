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

void instrument_pitch_offset(Instrument *instrument, int voice_id, double pitch_offset) {
    if (instrument->type == INSTR_SYNTH) {
        synth_pitch_offset(instrument->synth, voice_id, pitch_offset);
    }
}

void instrument_set_volume(Instrument *instrument, Uint8 volume) {
    instrument->volume = volume;
}

void instrument_modify_volume(Instrument *instrument, Sint8 volume_change) {
    if (instrument->volume + volume_change < 1) {
        instrument->volume = 1;
    } else if (instrument->volume + volume_change > 255) {
        instrument->volume = 255;
    } else {
        instrument->volume += volume_change;
    }
}


double instrument_poll(Instrument *instrument, double delta_time) {
    double sample = 0;
    if (instrument->type == INSTR_SYNTH) {
        sample = synth_poll(instrument->synth, delta_time);
    } else if (instrument->type == INSTR_SAMPLER) {
        sample = sampler_poll(instrument->sampler, delta_time);
    }
    if (instrument->volume > 0) {
        sample *= lookup_tracker_volume(instrument->volume);
    }
    return sample;
}
