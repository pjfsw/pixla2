#include "modulation.h"
#include <stdio.h>

void modulation_init(Modulation *modulation,
    Oscillator *oscillator1, Oscillator *oscillator2) {
    modulation->oscillator1 = oscillator1;
    modulation->oscillator2 = oscillator2;
}

void modulation_trigger(Modulation *modulation, double frequency) {
    lfo_trigger(&modulation->lfo1, frequency);
    lfo_trigger(&modulation->lfo2, frequency);
}

void modulation_off(Modulation *modulation) {
}

void modulation_transform(Modulation *modulation, double value, double delta_time) {

    double lfo1 = lfo_transform(&modulation->lfo1, value, delta_time);
    //double lfo2 = lfo_transform(&modulation->lfo2, value, delta_time);
    if (modulation->settings->modulation_target == MOD_OSCILLATOR_FM) {
        oscillator_set_fm(modulation->oscillator1, lfo1);
        oscillator_set_fm(modulation->oscillator2, lfo1);
    }
}
