#include "combiner.h"

void combiner_set_mode(Combiner *combiner, CombineMode combine_mode) {
    combiner->combine_mode = combine_mode;
}

void combiner_set_oscillator2_strength(Combiner *combiner, double strength) {
    combiner->oscillator2_strength = strength;
}

void combiner_trigger(void *user_data, double frequency) {
    Combiner* combiner = (Combiner*)user_data;
    combiner->frequency = frequency;
    oscillator_trigger(combiner->oscillator1, frequency);
    oscillator_trigger(combiner->oscillator2, frequency);
}

double combiner_transform(void *user_data, double value, double delta_time) {
    Combiner* combiner = (Combiner*)user_data;
    double v2 = oscillator_transform(combiner->oscillator2, value, delta_time);
    double v1 = 0;
    double a2 = combiner->oscillator2_strength;
    double a1 = 1.0-a2;

    switch (combiner->combine_mode) {
    case COMB_ADD:
        v1 = oscillator_transform(combiner->oscillator1, value, delta_time);
        return (a1*v1+a2*v2) * 1.4;
    case COMB_SUBTRACT:
        v1 = oscillator_transform(combiner->oscillator1, value, delta_time);
        return (a1*v1-a2*v2) * 1.4;
    case COMB_MULTIPLY:
        v1 = oscillator_transform(combiner->oscillator1, value, delta_time);
        return a1*v1+a2*v1*v2;
    case COMB_MODULATE:
        oscillator_trigger(
            combiner->oscillator1,
            combiner->frequency + combiner->frequency * a2 * v2);
        v1 = oscillator_transform(combiner->oscillator1, value, delta_time);
        return v1;
    }
    return 0.0;
}
