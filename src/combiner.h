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

typedef enum {
    STRENGTH_MANUAL,
    STRENGTH_VCA
} CombineStrengthMode;

typedef struct {
    CombineMode combine_mode;
    CombineStrengthMode strength_mode;
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
    Vca vca;
    double frequency;
    double amp;
} Combiner;

void combiner_set_ring_modulation(Combiner *combiner, double multiplier, double amount);

void combiner_trigger(void *user_data, double frequency);

double combiner_transform(void *user_data, double value, double delta_time);

void combiner_off(void *user_data);


#endif /* SRC_COMBINER_H_ */
