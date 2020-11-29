#include "selection_functions.h"

int* sf_synth_oscillator1_waveform(Synth *synth) {
    return &synth->oscillator_settings[0].waveform;
}

int* sf_synth_oscillator2_waveform(Synth *synth) {
    return &synth->oscillator_settings[1].waveform;
}

int *sf_synth_combiner_mode(Synth *synth) {
    return &synth->combiner_settings.combine_mode;
}

int *sf_synth_combiner_oscillator2_strength_mode(Synth *synth) {
    return &synth->combiner_settings.strength_mode;
}
