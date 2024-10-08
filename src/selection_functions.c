#include "selection_functions.h"
#include "synth.h"

int* sf_synth_oscillator1_waveform(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[0].waveform;
}

int* sf_synth_oscillator1_transpose(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[0].transpose;
}

int* sf_synth_oscillator1_phase_mode(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[0].phase_mode;
}


int* sf_synth_oscillator2_waveform(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[1].waveform;
}

int* sf_synth_oscillator2_transpose(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[1].transpose;
}

int* sf_synth_oscillator2_phase_mode(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[1].phase_mode;
}

int *sf_synth_combiner_mode(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return (int*)&synth->combiner_settings.combine_mode;
}

int *sf_synth_filter_vca_inverse(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->filter_vca_settings.inverse;
}

int *sf_synth_echo(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->echo_settings.sync_mode;
}

int* sf_synth_lfo1_waveform(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo[0].oscillator.waveform;
}

int* sf_synth_lfo2_waveform(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo[1].oscillator.waveform;
}

int* sf_synth_lfo1_target(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo_target[0];
}

int* sf_synth_lfo2_target(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo_target[1];
}

int *sf_synth_mod_vca_target(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.vca_target;
}

int *sf_synth_mod_vca_inverse(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.vca.inverse;
}
