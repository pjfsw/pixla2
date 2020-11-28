#ifndef SRC_PARAMETER_FUNCTIONS_H_
#define SRC_PARAMETER_FUNCTIONS_H_

#include "synth.h"

// Voice
double *pf_synth_voice_attack(Synth *synth);

double *pf_synth_voice_decay(Synth *synth);

double *pf_synth_voice_sustain(Synth *synth);

double *pf_synth_voice_release(Synth *synth);

// Combiner
double *pf_synth_comb_detune(Synth *synth);

double *pf_synth_comb_strength(Synth *synth);

double *pf_synth_comb_oscillator_scale(Synth *synth);

// Filter
double *pf_synth_filter_f(Synth *synth);

double *pf_synth_filter_q(Synth *synth);

double *pf_synth_filter_attack(Synth *synth);

double *pf_synth_filter_decay(Synth *synth);

double *pf_synth_filter_sustain(Synth *synth);

double *pf_synth_filter_release(Synth *synth);

// Modulation

#endif /* SRC_PARAMETER_FUNCTIONS_H_ */
