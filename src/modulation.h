#ifndef SRC_MODULATION_H_
#define SRC_MODULATION_H_

#include "modulation_target.h"
#include "lfo.h"
#include "oscillator.h"
#include "filter.h"
#include "combiner.h"

#define NUMBER_OF_MODULATORS 2

typedef enum {
    LFO_OSC1_OSC2,
    LFO_FILTER,
    LFO_PHASE,
    LFO_OSC2_MIX
} LfoTarget;

typedef enum {
    MOD_VCA_OFF,
    MOD_VCA_OSC1_OSC2,
    MOD_VCA_PHASE,
    MOD_VCA_OSC2_MIX,
    MOD_VCA_RING_FREQ,
    MOD_VCA_RING_AMT,
    MOD_VCA_RING

} VcaTarget;

typedef struct {
    VcaSettings vca;
    int vca_target;
    Uint8 vca_strength;
    LfoSettings lfo[NUMBER_OF_MODULATORS];
    int lfo_target[NUMBER_OF_MODULATORS];
} ModulationSettings;

typedef struct {
    ModulationSettings *settings;
    Lfo lfo[NUMBER_OF_MODULATORS];
    Oscillator *oscillator1;
    Oscillator *oscillator2;
    Vca vca;
    Filter *filter;
    Combiner *combiner;
} Modulation;

void modulation_init(Modulation *modulation,
    Oscillator *oscillator1, Oscillator *oscillator2,
    Filter *filter, Combiner *combiner);

void modulation_trigger(Modulation *modulation, double frequency);

void modulation_off(Modulation *modulation);

void modulation_transform(Modulation *modulation, double value, double delta_time);

#endif /* SRC_MODULATION_H_ */
