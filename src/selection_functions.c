#include "selection_functions.h"
#include "synth.h"

int* sf_synth_oscillator1_waveform(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[0].waveform;
}

int* sf_synth_oscillator2_waveform(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[1].waveform;
}

int* sf_synth_oscillator2_transpose(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[1].transpose;
}


int *sf_synth_combiner_mode(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return (int*)&synth->combiner_settings.combine_mode;
}

int *sf_synth_combiner_oscillator2_strength_mode(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return (int*)&synth->combiner_settings.strength_mode;
}

int *sf_synth_combiner_vca_inverse(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->combiner_vca_settings.inverse;
}

int *sf_synth_filter_vca_inverse(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->filter_vca_settings.inverse;
}

int *sf_synth_echo(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->use_echo;
}

int* sf_synth_lfo1_waveform(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo.oscillator.waveform;
}


