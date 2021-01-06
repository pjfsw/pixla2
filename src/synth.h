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
    int portamento;
    Uint8 velocity;
} Voice;

typedef struct {
    VcaSettings voice_vca_settings;
    CombinerSettings combiner_settings;
    VcaSettings filter_vca_settings;
    FilterSettings filter_settings;
    OscillatorSettings oscillator_settings[NUMBER_OF_OSCILLATORS];
    ModulationSettings modulation_settings;
    EchoSettings echo_settings;
    Uint8 master_level;
} SynthSettings;

typedef struct {
    SynthSettings *settings;
    // Internals
    Voice *voices;
    int number_of_voices;
    double volume_reduction;
    int next_voice;
    Echo echo;
} Synth;

void synth_settings_set_default(SynthSettings* settings);

Synth *synth_create();

void synth_destroy(Synth *synth);

int synth_note_on(Synth *synth, int note, int velocity);

void synth_note_off(Synth *synth, int voice_id);

void synth_off(Synth *synth);

void synth_pitch_offset(Synth *synth, int voice_id, double offset);

double synth_poll(Synth *synth, double delta_time);

#endif /* SRC_SYNTH_H_ */
