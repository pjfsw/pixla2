#ifndef SRC_PARAMETER_FUNCTIONS_H_
#define SRC_PARAMETER_FUNCTIONS_H_

#include "synth.h"

double *pf_synth_voice_attack(Synth *synth);

double *pf_synth_voice_decay(Synth *synth);

double *pf_synth_voice_sustain(Synth *synth);

double *pf_synth_voice_release(Synth *synth);

double *pf_synth_comb_detune(Synth *synth);

double *pf_synth_comb_strength(Synth *synth);

double *pf_synth_comb_oscillator_scale(Synth *synth);

#endif /* SRC_PARAMETER_FUNCTIONS_H_ */
