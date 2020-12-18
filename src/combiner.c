#include <math.h>
#include "combiner.h"
#include "vca.h"
#include "lookup_tables.h"

void combiner_trigger(void *user_data, double frequency) {
    Combiner* combiner = (Combiner*)user_data;
    combiner->frequency = frequency;
    double detune_up = pow(2, lookup_detune_fine(combiner->settings->detune)/12);
    double detune_down = pow(2, -lookup_detune_fine(combiner->settings->detune)/12);
    oscillator_trigger(combiner->oscillator1,
        detune_up * frequency);
    oscillator_trigger(combiner->oscillator2,
        detune_down * frequency);
    vca_trigger(&combiner->vca, frequency);
}

void combiner_off(void *user_data) {
    Combiner* combiner = (Combiner*)user_data;
    vca_off(&combiner->vca);
}

void combiner_set_ring_modulation(Combiner *combiner, double multiplier, double amount) {
    combiner->ring_mod_amount = amount;
    combiner->ring_mod_multiplier = multiplier;
}

double combiner_transform(void *user_data, double value, double delta_time) {
    Combiner* combiner = (Combiner*)user_data;
    double osc2 = oscillator_transform(combiner->oscillator2, value, delta_time);
    double osc = 0;
    double vca_strength = vca_transform(&combiner->vca, value, delta_time);
    double amp2 = lookup_mix_balance(combiner->settings->oscillator2_strength);

    if (combiner->settings->strength_mode == STRENGTH_VCA) {
        amp2 *= vca_strength;
    }

    if (combiner->settings->combine_mode == COMB_ADD) {
        osc = oscillator_transform(combiner->oscillator1, value, delta_time);
        osc += amp2 * osc2;
    } else if (combiner->settings->combine_mode == COMB_MODULATE) {
        oscillator_trigger(
            combiner->oscillator1,
            combiner->frequency * pow(2, 0.2 * amp2 * osc2));
        osc = oscillator_transform(combiner->oscillator1, value, delta_time);
    }
    return osc * (1.0 - combiner->ring_mod_amount) +
        osc * combiner->ring_mod_amount * combiner->ring_mod_multiplier;
}
