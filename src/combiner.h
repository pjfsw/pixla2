#ifndef SRC_COMBINER_H_
#define SRC_COMBINER_H_

#include <SDL2/SDL.h>

#include "oscillator.h"
#include "vca.h"
#include "lfo.h"


typedef enum {
    COMB_ADD,
    COMB_MODULATE
} CombineMode;

typedef struct {
    CombineMode combine_mode;
    Uint8 oscillator2_strength;
    Uint8 detune;
    OscillatorSettings ring_oscillator;
    Uint8 ring_mod_freq;
    Uint8 ring_mod_amount;
} CombinerSettings;

typedef struct {
    CombinerSettings *settings;
    Oscillator *oscillator1;
    Oscillator *oscillator2;
    Oscillator ring_modulator;
    double frequency;
    double amp;
    double osc2_mix;
    double ring_freq_mod;
    double ring_amount_mod;
} Combiner;

void combiner_trigger(void *user_data, double frequency, Uint8 velocity);

void combiner_set_osc2_mix(Combiner *combiner, double mix);

void combiner_set_ring_freq_mod(Combiner *combiner, double ring_freq_mod);

void combiner_set_ring_amount_mod(Combiner *combiner, double ring_amount_mod);

double combiner_transform(void *user_data, double value, double delta_time);

void combiner_off(void *user_data);


#endif /* SRC_COMBINER_H_ */
