#ifndef SRC_MODULATION_H_
#define SRC_MODULATION_H_

#include "modulation_target.h"
#include "lfo.h"
#include "oscillator.h"
#include "filter.h"

#define NUMBER_OF_MODULATORS 2

typedef enum {
    MOD_OSC1_OSC2,
    MOD_FILTER,
    MOD_PHASE
} ModulationTarget;


typedef struct {
    LfoSettings lfo[NUMBER_OF_MODULATORS];
    int target[NUMBER_OF_MODULATORS];
} ModulationSettings;

typedef struct {
    ModulationSettings *settings;
    Lfo lfo[NUMBER_OF_MODULATORS];
    Oscillator *oscillator1;
    Oscillator *oscillator2;
    Filter *filter;
} Modulation;

void modulation_init(Modulation *modulation,
    Oscillator *oscillator1, Oscillator *oscillator2,
    Filter *filter);


void modulation_trigger(Modulation *modulation, double frequency);

void modulation_off(Modulation *modulation);

void modulation_transform(Modulation *modulation, double value, double delta_time);

#endif /* SRC_MODULATION_H_ */
