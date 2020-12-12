#include "parameter_functions.h"
#include "synth.h"

double *pf_synth_voice_attack(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->voice_vca_settings.attack;
}

double *pf_synth_voice_decay(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->voice_vca_settings.decay;
}

double *pf_synth_voice_sustain(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->voice_vca_settings.sustain;
}

double *pf_synth_voice_release(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->voice_vca_settings.release;
}

double *pf_synth_comb_detune(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->combiner_settings.detune;
}

double *pf_synth_comb_strength(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->combiner_settings.oscillator2_strength;
}

double *pf_synth_comb_attack(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->combiner_vca_settings.attack;
}

double *pf_synth_comb_decay(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->combiner_vca_settings.decay;
}

double *pf_synth_comb_sustain(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->combiner_vca_settings.sustain;
}

double *pf_synth_comb_release(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->combiner_vca_settings.release;
}

double *pf_synth_filter_f(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->filter_settings.f;
}

double *pf_synth_filter_q(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->filter_settings.q;
}

double *pf_synth_filter_attack(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->filter_vca_settings.attack;
}

double *pf_synth_filter_decay(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->filter_vca_settings.decay;
}

double *pf_synth_filter_sustain(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->filter_vca_settings.sustain;
}

double *pf_synth_filter_release(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->filter_vca_settings.release;
}

double *pf_synth_lfo1_frequency(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->modulation_settings.lfo[0].frequency;
}

double *pf_synth_lfo1_amount(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->modulation_settings.lfo[0].amount;

}

double *pf_synth_lfo1_delay(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->modulation_settings.lfo[0].delay;
}

double *pf_synth_master_level(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->master_level;
}

double *pf_synth_echo_delay(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->echo.echo_time;
}

double *pf_synth_echo_wetness(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->echo.wetness;
}

double *pf_synth_echo_feedback(void *user_data) {
    Synth *synth = (Synth*)user_data;
    return &synth->echo.feedback;
}
