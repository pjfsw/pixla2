#ifndef SRC_SYNTH_H_
#define SRC_SYNTH_H_

#include "processor.h"
#include "oscillator.h"
#include "filter.h"
#include "vca.h"
#include "combiner.h"

typedef struct {
    Oscillator oscillator;
    Oscillator oscillator2;
    Filter filter;
    Vca vca;
    Combiner combiner;
    Processor processor;
    int note;
} Voice;

typedef struct {
    Voice *voices;
    int number_of_voices;
    int next_voice;
    ProcessorStage *post_stages;
    int number_of_post_stages;
} Synth;

Synth *synth_create();

void synth_destroy(Synth *synth);

void synth_note_on(Synth *synth, int note);

void synth_note_off(Synth *synth, int note);

void synth_set_waveform(Synth *synth, Waveform waveform);

float synth_poll(Synth *synth, double delta_time);

#endif /* SRC_SYNTH_H_ */
