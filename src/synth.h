#ifndef SRC_SYNTH_H_
#define SRC_SYNTH_H_

#include "oscillator.h"

typedef struct {
    Oscillator oscillators[4];
    int number_of_oscillators;
    int nextOscillator;
} Synth;

Synth *synth_create();

void synth_destroy(Synth *synth);

void synth_play(Synth *synth, int note);

float synth_poll(Synth *synth);

#endif /* SRC_SYNTH_H_ */
