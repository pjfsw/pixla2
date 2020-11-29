#include <math.h>
#include "combiner.h"
#include "vca.h"

void combiner_trigger(void *user_data, double frequency) {
    Combiner* combiner = (Combiner*)user_data;
    combiner->frequency = frequency;
    double detune_up = pow(2, combiner->settings->detune/12);
    double detune_down = pow(2, -combiner->settings->detune/12);
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

double combiner_transform(void *user_data, double value, double delta_time) {
    Combiner* combiner = (Combiner*)user_data;
    double v2 = oscillator_transform(combiner->oscillator2, value, delta_time);
    double v1 = 0;
    double vca_strength = vca_transform(&combiner->vca, value, delta_time);
    double a2;
    if (combiner->settings->strength_mode == STRENGTH_VCA) {
        a2 = vca_strength;
    } else {
        a2 = combiner->settings->oscillator2_strength;
    }
    double a1 = 1.0-a2;
    switch (combiner->settings->combine_mode) {
    case COMB_ADD:
        v1 = oscillator_transform(combiner->oscillator1, value, delta_time);
        return (a1*v1+a2*v2);
    case COMB_MULTIPLY:
        v1 = oscillator_transform(combiner->oscillator1, value, delta_time);
        return a1*v1+a2*v1*v2;
    case COMB_MODULATE:
        oscillator_trigger(
            combiner->oscillator1,
            combiner->frequency * pow(2, a2 * v2));
        v1 = oscillator_transform(combiner->oscillator1, value, delta_time);
        return v1;
    }
    return 0.0;
}
