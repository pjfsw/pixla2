#include "parameter_functions.h"

double *pf_synth_voice_attack(Synth *synth) {
    return &synth->voice_vca_settings.attack;
}

double *pf_synth_voice_decay(Synth *synth) {
    return &synth->voice_vca_settings.decay;
}

double *pf_synth_voice_sustain(Synth *synth) {
    return &synth->voice_vca_settings.sustain;
}

double *pf_synth_voice_release(Synth *synth) {
    return &synth->voice_vca_settings.release;
}

double *pf_synth_comb_detune(Synth *synth) {
    return &synth->combiner_settings.detune;
}

double *pf_synth_comb_strength(Synth *synth) {
    return &synth->combiner_settings.oscillator2_strength;
}

double *pf_synth_comb_oscillator_scale(Synth *synth) {
    return &synth->combiner_settings.oscillator2_scale;
}

double *pf_synth_filter_f(Synth *synth) {
    return &synth->filter_settings.f;
}

double *pf_synth_filter_q(Synth *synth) {
    return &synth->filter_settings.q;
}

double *pf_synth_filter_attack(Synth *synth) {
    return &synth->filter_vca_settings.attack;
}

double *pf_synth_filter_decay(Synth *synth) {
    return &synth->filter_vca_settings.decay;
}

double *pf_synth_filter_sustain(Synth *synth) {
    return &synth->filter_vca_settings.sustain;
}

double *pf_synth_filter_release(Synth *synth) {
    return &synth->filter_vca_settings.release;
}

