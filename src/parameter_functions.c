#include <stdio.h>
#include "parameter_functions.h"
#include "synth.h"
#include <SDL2/SDL.h>

Uint8 *pf_synth_oscillator1_phase(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[0].phase;
}

Uint8 *pf_synth_oscillator2_phase(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->oscillator_settings[1].phase;
}


Uint8 *pf_synth_voice_attack(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->voice_vca_settings.attack;
}

Uint8 *pf_synth_voice_decay(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->voice_vca_settings.decay;
}

Uint8 *pf_synth_voice_sustain(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->voice_vca_settings.sustain;
}

Uint8 *pf_synth_voice_release(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->voice_vca_settings.release;
}

Uint8 *pf_synth_comb_detune(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->combiner_settings.detune;
}

Uint8 *pf_synth_comb_strength(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->combiner_settings.oscillator2_strength;
}

Uint8 *pf_synth_comb_attack(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->combiner_vca_settings.attack;
}

Uint8 *pf_synth_comb_decay(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->combiner_vca_settings.decay;
}

Uint8 *pf_synth_comb_sustain(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->combiner_vca_settings.sustain;
}

Uint8 *pf_synth_comb_release(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->combiner_vca_settings.release;
}

Uint8 *pf_synth_filter_f(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->filter_settings.f;
}

Uint8 *pf_synth_filter_q(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->filter_settings.q;
}

Uint8 *pf_synth_filter_attack(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->filter_vca_settings.attack;
}

Uint8 *pf_synth_filter_decay(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->filter_vca_settings.decay;
}

Uint8 *pf_synth_filter_sustain(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->filter_vca_settings.sustain;
}

Uint8 *pf_synth_filter_release(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->filter_vca_settings.release;
}

Uint8 *pf_synth_lfo1_frequency(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo[0].frequency;
}

Uint8 *pf_synth_lfo1_amount(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo[0].amount;

}

Uint8 *pf_synth_lfo1_delay(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo[0].delay;
}

Uint8 *pf_synth_lfo2_frequency(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo[1].frequency;
}

Uint8 *pf_synth_lfo2_amount(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo[1].amount;

}

Uint8 *pf_synth_lfo2_delay(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.lfo[1].delay;
}



Uint8 *pf_synth_master_level(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->master_level;
}

Uint8 *pf_synth_echo_delay(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->echo.echo_time;
}

Uint8 *pf_synth_echo_wetness(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->echo.wetness;
}

Uint8 *pf_synth_echo_feedback(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->echo.feedback;
}
