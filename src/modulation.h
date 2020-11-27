#ifndef SRC_MODULATION_H_
#define SRC_MODULATION_H_

#include "modulation_target.h"
#include "lfo.h"
#include "oscillator.h"

typedef struct {
    ModulationTarget modulation_target;
    LfoSettings lfo[2];
} ModulationSettings;

typedef struct {
    ModulationSettings *settings;

    Lfo lfo1;
    Lfo lfo2;
    Oscillator *oscillator1;
    Oscillator *oscillator2;
} Modulation;

void modulation_init(Modulation *modulation,
    Oscillator *oscillator1, Oscillator *oscillator2);


void modulation_trigger(Modulation *modulation, double frequency);

void modulation_off(Modulation *modulation);

void modulation_transform(Modulation *modulation, double value, double delta_time);

#endif /* SRC_MODULATION_H_ */
