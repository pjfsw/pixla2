#ifndef SRC_SYNTH_H_
#define SRC_SYNTH_H_

#include "processor.h"

typedef struct {
    Processor *processors;
    int *notes;
    int number_of_processors;
    int nextProcessor;
} Synth;

Synth *synth_create();

void synth_destroy(Synth *synth);

void synth_note_on(Synth *synth, int note);

void synth_note_off(Synth *synth, int note);

float synth_poll(Synth *synth, double delta_time);

#endif /* SRC_SYNTH_H_ */
