#include <stdio.h>

#include "modulation.h"
#include "lookup_tables.h"

void modulation_init(Modulation *modulation,
    Oscillator *oscillator1, Oscillator *oscillator2,
    Filter *filter) {
    modulation->oscillator1 = oscillator1;
    modulation->oscillator2 = oscillator2;
    modulation->filter = filter;
}

void modulation_trigger(Modulation *modulation, double frequency) {
    for (int i = 0; i < NUMBER_OF_MODULATORS; i++) {
        lfo_trigger(&modulation->lfo[i], frequency);
    }
}

void modulation_off(Modulation *modulation) {
}

void modulation_transform(Modulation *modulation, double value, double delta_time) {
    double osc1_osc2 = 0;
    double filter = 0;
    double phase = 0;
    for (int i = 0; i < NUMBER_OF_MODULATORS; i++) {
        double lfo = lfo_transform(&modulation->lfo[i], value, delta_time);
        switch (modulation->settings->target[i]) {
        case MOD_OSC1_OSC2:
            osc1_osc2 += lfo;
            break;
        case MOD_PHASE:
            phase += lfo;
            break;
        case MOD_FILTER:
            filter += lfo;
            break;
        }
    }

    oscillator_set_fm(modulation->oscillator1, osc1_osc2);
    oscillator_set_fm(modulation->oscillator2, osc1_osc2);
    oscillator_set_pm(modulation->oscillator1, phase);
    oscillator_set_pm(modulation->oscillator2, phase);
    filter_mod(modulation->filter, filter);


}
