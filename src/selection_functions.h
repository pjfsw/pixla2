#ifndef SELECTION_FUNCTIONS_H_
#define SELECTION_FUNCTIONS_H_

#include <SDL2/SDL.h>

// Oscillator
int* sf_synth_oscillator1_waveform(void *user_data);

int* sf_synth_oscillator1_phase_mode(void *user_data);

int* sf_synth_oscillator1_transpose(void *user_data);

int* sf_synth_oscillator2_waveform(void *user_data);

int* sf_synth_oscillator2_phase_mode(void *user_data);

int* sf_synth_oscillator2_transpose(void *user_data);

// Combiner
int *sf_synth_combiner_mode(void *user_data);

// VCAs
int *sf_synth_mod_vca_inverse(void *user_data);

int *sf_synth_mod_vca_target(void *user_data);

int *sf_synth_filter_vca_inverse(void *user_data);

int *sf_synth_echo(void *user_data);

// Modulation
int* sf_synth_lfo1_waveform(void *user_data);

int* sf_synth_lfo2_waveform(void *user_data);

int* sf_synth_lfo1_target(void *user_data);

int* sf_synth_lfo2_target(void *user_data);

#endif /* SELECTION_FUNCTIONS_H_ */
