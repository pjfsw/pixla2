#include <stdio.h>
#include <SDL2/SDL.h>
#include "parameter_functions.h"
#include "synth.h"
#include "mixer.h"

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

Uint8 *pf_synth_comb_ring_freq(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->combiner_settings.ring_mod_freq;
}

Uint8 *pf_synth_comb_ring_amount(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->combiner_settings.ring_mod_amount;
}

Uint8 *pf_synth_mod_attack(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.vca.attack;
}

Uint8 *pf_synth_mod_decay(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.vca.decay;
}

Uint8 *pf_synth_mod_sustain(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.vca.sustain;
}

Uint8 *pf_synth_mod_release(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.vca.release;
}

Uint8 *pf_synth_mod_vca_strength(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->modulation_settings.vca_strength;
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
    return &synth->echo_settings.echo_time;
}

Uint8 *pf_synth_echo_wetness(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->echo_settings.wetness;
}

Uint8 *pf_synth_echo_feedback(void *user_data) {
    SynthSettings *synth = (SynthSettings *)user_data;
    return &synth->echo_settings.feedback;
}

Uint8 *pf_mixer_master_level(void *user_data) {
    MixerSettings *mixer = (MixerSettings *)user_data;
    return &mixer->master_volume;
}

UiParameterFunc pf_mixer_instr_level(int instr) {
    if (instr == 0) {
        return pf_mixer_level_instr1;
    } else if (instr == 1) {
        return pf_mixer_level_instr2;
    } else if (instr == 2) {
        return pf_mixer_level_instr3;
    } else if (instr == 3) {
        return pf_mixer_level_instr4;
    } else if (instr == 4) {
        return pf_mixer_level_instr5;
    } else if (instr == 5) {
        return pf_mixer_level_instr6;
    } else if (instr == 6) {
        return pf_mixer_level_instr7;
    } else if (instr == 7) {
        return pf_mixer_level_instr8;
    }
    return NULL;
}

Uint8 *pf_mixer_level_instr1(void *user_data) {
    MixerSettings *mixer = (MixerSettings *)user_data;
    return &mixer->instr_volume[0];
}

Uint8 *pf_mixer_level_instr2(void *user_data) {
    MixerSettings *mixer = (MixerSettings *)user_data;
    return &mixer->instr_volume[1];
}

Uint8 *pf_mixer_level_instr3(void *user_data) {
    MixerSettings *mixer = (MixerSettings *)user_data;
    return &mixer->instr_volume[2];
}

Uint8 *pf_mixer_level_instr4(void *user_data) {
    MixerSettings *mixer = (MixerSettings *)user_data;
    return &mixer->instr_volume[3];
}

Uint8 *pf_mixer_level_instr5(void *user_data) {
    MixerSettings *mixer = (MixerSettings *)user_data;
    return &mixer->instr_volume[4];
}

Uint8 *pf_mixer_level_instr6(void *user_data) {
    MixerSettings *mixer = (MixerSettings *)user_data;
    return &mixer->instr_volume[5];
}

Uint8 *pf_mixer_level_instr7(void *user_data) {
    MixerSettings *mixer = (MixerSettings *)user_data;
    return &mixer->instr_volume[6];
}

Uint8 *pf_mixer_level_instr8(void *user_data) {
    MixerSettings *mixer = (MixerSettings *)user_data;
    return &mixer->instr_volume[7];
}

