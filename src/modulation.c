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
    lfo_trigger(&modulation->lfo, frequency);
}

void modulation_off(Modulation *modulation) {
}

void modulation_transform(Modulation *modulation, double value, double delta_time) {
    double lfo = lfo_transform(&modulation->lfo, value, delta_time);
    oscillator_set_fm(modulation->oscillator1, lfo * lookup_mod_frequency(modulation->settings->oscillator1));
    oscillator_set_fm(modulation->oscillator2, lfo * lookup_mod_frequency(modulation->settings->oscillator2));
    filter_mod(modulation->filter, lfo * lookup_mod_frequency(modulation->settings->filter));
}
