#ifndef SRC_MODULATION_H_
#define SRC_MODULATION_H_

#include "modulation_target.h"
#include "lfo.h"
#include "oscillator.h"
#include "filter.h"

typedef struct {
    LfoSettings lfo;
    Uint8 oscillator1;
    Uint8 oscillator2;
    Uint8 filter;
} ModulationSettings;

typedef struct {
    ModulationSettings *settings;
    Lfo lfo;
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
