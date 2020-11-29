#ifndef SRC_SYNTH_H_
#define SRC_SYNTH_H_

#include "processor.h"
#include "oscillator.h"
#include "filter.h"
#include "vca.h"
#include "combiner.h"
#include "lfo.h"
#include "modulation.h"
#include "echo.h"

#define NUMBER_OF_OSCILLATORS 2

typedef struct {
    Oscillator oscillators[NUMBER_OF_OSCILLATORS];
    Combiner combiner;
    Filter filter;
    Vca voice_vca;
    Processor processor;
    Modulation modulation;
    int note;
} Voice;

typedef struct {
    // Setings
    VcaSettings voice_vca_settings;
    CombinerSettings combiner_settings;
    VcaSettings combiner_vca_settings;
    VcaSettings filter_vca_settings;
    FilterSettings filter_settings;
    OscillatorSettings oscillator_settings[NUMBER_OF_OSCILLATORS];
    ModulationSettings modulation_settings;
    double master_level;
    int use_echo;
    // Internals
    Voice *voices;
    int number_of_voices;
    int next_voice;
    Echo echo;
} Synth;

Synth *synth_create();

void synth_destroy(Synth *synth);

void synth_note_on(Synth *synth, int note);

void synth_note_off(Synth *synth, int note);

double synth_poll(Synth *synth, double delta_time);

#endif /* SRC_SYNTH_H_ */
