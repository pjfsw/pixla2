#ifndef SRC_PARAMETER_FUNCTIONS_H_
#define SRC_PARAMETER_FUNCTIONS_H_

#include <SDL2/SDL.h>

// SYNTH

// Oscillator
Uint8 *pf_synth_oscillator1_phase(void *user_data);

Uint8 *pf_synth_oscillator2_phase(void *user_data);

// Voice VCA
Uint8 *pf_synth_voice_attack(void *user_data);

Uint8 *pf_synth_voice_decay(void *user_data);

Uint8 *pf_synth_voice_sustain(void *user_data);

Uint8 *pf_synth_voice_release(void *user_data);

// Combiner
Uint8 *pf_synth_comb_detune(void *user_data);

Uint8 *pf_synth_comb_strength(void *user_data);

Uint8 *pf_synth_comb_ring_freq(void *user_data);

Uint8 *pf_synth_comb_ring_amount(void *user_data);

// Filter
Uint8 *pf_synth_filter_f(void *user_data);

Uint8 *pf_synth_filter_q(void *user_data);

Uint8 *pf_synth_filter_attack(void *user_data);

Uint8 *pf_synth_filter_decay(void *user_data);

Uint8 *pf_synth_filter_sustain(void *user_data);

Uint8 *pf_synth_filter_release(void *user_data);

// Modulation
Uint8 *pf_synth_lfo1_frequency(void *user_data);

Uint8 *pf_synth_lfo1_amount(void *user_data);

Uint8 *pf_synth_lfo1_delay(void *user_data);

Uint8 *pf_synth_lfo2_frequency(void *user_data);

Uint8 *pf_synth_lfo2_amount(void *user_data);

Uint8 *pf_synth_lfo2_delay(void *user_data);

Uint8 *pf_synth_mod_attack(void *user_data);

Uint8 *pf_synth_mod_decay(void *user_data);

Uint8 *pf_synth_mod_sustain(void *user_data);

Uint8 *pf_synth_mod_release(void *user_data);

Uint8 *pf_synth_mod_vca_strength(void *user_data);


// Synth master
Uint8 *pf_synth_master_level(void *user_data);

// Echo
Uint8 *pf_synth_echo_delay(void *user_data);

Uint8 *pf_synth_echo_wetness(void *user_data);

Uint8 *pf_synth_echo_feedback(void *user_data);

// MIXER
Uint8 *pf_mixer_master_level(void *user_data);
Uint8 *pf_mixer_level_instr1(void *user_data);
Uint8 *pf_mixer_level_instr2(void *user_data);
Uint8 *pf_mixer_level_instr3(void *user_data);
Uint8 *pf_mixer_level_instr4(void *user_data);
Uint8 *pf_mixer_level_instr5(void *user_data);
Uint8 *pf_mixer_level_instr6(void *user_data);
Uint8 *pf_mixer_level_instr7(void *user_data);
Uint8 *pf_mixer_level_instr8(void *user_data);

#endif /* SRC_PARAMETER_FUNCTIONS_H_ */
