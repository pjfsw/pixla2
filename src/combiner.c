#include "combiner.h"

void combiner_trigger(void *user_data, double frequency) {
    Combiner* combiner = (Combiner*)user_data;
    combiner->frequency = frequency;
    oscillator_trigger(combiner->oscillator1, frequency);
    oscillator_trigger(combiner->oscillator2, frequency*4);
}

double combiner_transform(void *user_data, double value, double delta_time) {
    Combiner* combiner = (Combiner*)user_data;
    double v2 = oscillator_transform(combiner->oscillator2, value, delta_time);
    double v1 = oscillator_transform(combiner->oscillator1, value, delta_time);
    return (v1+v2)*0.7;
}
