#ifndef SRC_PARAMETER_FUNCTIONS_H_
#define SRC_PARAMETER_FUNCTIONS_H_

#include <SDL2/SDL.h>

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

Uint8 *pf_synth_comb_attack(void *user_data);

Uint8 *pf_synth_comb_decay(void *user_data);

Uint8 *pf_synth_comb_sustain(void *user_data);

Uint8 *pf_synth_comb_release(void *user_data);


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

Uint8 *pf_synth_mod_osc1(void *user_data);

Uint8 *pf_synth_mod_osc2(void *user_data);

Uint8 *pf_synth_mod_filter(void *user_data);

// Mixer
Uint8 *pf_synth_master_level(void *user_data);

// Echo
Uint8 *pf_synth_echo_delay(void *user_data);

Uint8 *pf_synth_echo_wetness(void *user_data);

Uint8 *pf_synth_echo_feedback(void *user_data);

#endif /* SRC_PARAMETER_FUNCTIONS_H_ */
