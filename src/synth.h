#ifndef SRC_SYNTH_H_
#define SRC_SYNTH_H_

#include <SDL2/SDL.h>
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
    Uint8 velocity;
} Voice;

typedef struct {
    VcaSettings voice_vca_settings;
    CombinerSettings combiner_settings;
    VcaSettings filter_vca_settings;
    FilterSettings filter_settings;
    OscillatorSettings oscillator_settings[NUMBER_OF_OSCILLATORS];
    ModulationSettings modulation_settings;
    Uint8 master_level;
    int use_echo;
    Echo echo;
} SynthSettings;

typedef struct {
    SynthSettings settings;
    // Internals
    Voice *voices;
    int number_of_voices;
    double volume_reduction;
    int next_voice;
} Synth;

Synth *synth_create();

void synth_destroy(Synth *synth);

int synth_note_on(Synth *synth, int note, int velocity);

void synth_note_off(Synth *synth, int voice_id);

void synth_off(Synth *synth);

double synth_poll(Synth *synth, double delta_time);

#endif /* SRC_SYNTH_H_ */
