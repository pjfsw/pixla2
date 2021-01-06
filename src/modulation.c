#include <stdio.h>

#include "modulation.h"
#include "lookup_tables.h"

void modulation_init(Modulation *modulation,
    Oscillator *oscillator1, Oscillator *oscillator2,
    Filter *filter, Combiner *combiner) {
    modulation->oscillator1 = oscillator1;
    modulation->oscillator2 = oscillator2;
    modulation->filter = filter;
    modulation->combiner = combiner;
}

void modulation_trigger(Modulation *modulation, double frequency) {
    for (int i = 0; i < NUMBER_OF_MODULATORS; i++) {
        lfo_trigger(&modulation->lfo[i], frequency);
    }
    vca_trigger(&modulation->vca, frequency, 255);
}

void modulation_off(Modulation *modulation) {
    vca_off(&modulation->vca);
}

void modulation_transform(Modulation *modulation, double value, double delta_time) {
    double osc1_osc2 = 0;
    double filter = 0;
    double phase = 0;
    double osc2_mix = 1.0;
    double vca = vca_transform(&modulation->vca, value, delta_time);
    double vca_strength = lookup_mod_vca_strength(modulation->settings->vca_strength);
    double ring_freq = 0.0;
    double ring_amnt = 1.0;

    bool osc1_osc2_set = false;
    bool phase_set = false;
    bool osc2_mix_set = false;
    switch (modulation->settings->vca_target) {
    case MOD_VCA_PHASE:
        phase = 2.0 * vca_strength * (vca - 0.5);
        phase_set = true;
        break;
    case MOD_VCA_OSC2_MIX:
        osc2_mix = (1-vca_strength) + vca_strength * vca;
        osc2_mix_set = true;
        break;
    case MOD_VCA_RING:
        ring_freq = 16.0 *  vca_strength * (vca-0.5);
        ring_amnt = vca * vca_strength;
        break;
    case MOD_VCA_RING_FREQ:
        ring_freq = 16.0 *  vca_strength * (vca-0.5);
        break;
    case MOD_VCA_RING_AMT:
        ring_amnt = vca * vca_strength;
        break;
    default:
        break;
    }

    for (int i = 0; i < NUMBER_OF_MODULATORS; i++) {
        double lfo = lfo_transform(&modulation->lfo[i], value, delta_time);
        switch (modulation->settings->lfo_target[i]) {
        case LFO_OSC1_OSC2:
            if (!osc1_osc2_set) {
                osc1_osc2 += lfo;
            }
            break;
        case LFO_PHASE:
            if (!phase_set) {
                phase += lfo;
            }
            break;
        case LFO_FILTER:
            filter += lfo;
            break;
        case LFO_OSC2_MIX:
            if (!osc2_mix_set) {
                osc2_mix *= (lfo+1.0);
            }
            break;
        }
    }

    oscillator_set_fm(modulation->oscillator1, osc1_osc2);
    oscillator_set_fm(modulation->oscillator2, osc1_osc2);
    oscillator_set_pm(modulation->oscillator1, phase);
    oscillator_set_pm(modulation->oscillator2, phase);
    combiner_set_osc2_mix(modulation->combiner, osc2_mix);
    combiner_set_ring_freq_mod(modulation->combiner, ring_freq);
    combiner_set_ring_amount_mod(modulation->combiner, ring_amnt);
    filter_mod(modulation->filter, filter);


}
