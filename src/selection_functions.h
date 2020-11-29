#ifndef SELECTION_FUNCTIONS_H_
#define SELECTION_FUNCTIONS_H_

#include <SDL2/SDL.h>
#include "synth.h"

// Oscillator
int* sf_synth_oscillator1_waveform(Synth *synth);

int* sf_synth_oscillator2_waveform(Synth *synth);

// Combiner
int *sf_synth_combiner_mode(Synth *synth);

int *sf_synth_combiner_oscillator2_strength_mode(Synth *synth);

// VCAs
int *sf_synth_combiner_vca_inverse(Synth *synth);

int *sf_synth_filter_vca_inverse(Synth *synth);

int *sf_synth_echo(Synth *synth);

#endif /* SELECTION_FUNCTIONS_H_ */
