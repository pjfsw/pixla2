#include "selection_functions.h"

int* sf_synth_oscillator1_waveform(Synth *synth) {
    return &synth->oscillator_settings[0].waveform;
}

int* sf_synth_oscillator2_waveform(Synth *synth) {
    return &synth->oscillator_settings[1].waveform;
}
