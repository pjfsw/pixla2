#include "lfo.h"
#include "oscillator.h"
#include "lookup_tables.h"

void lfo_trigger(void *user_data, double frequency) {
    Lfo *lfo = (Lfo*)user_data;
    lfo->t = 0;
    oscillator_trigger(&lfo->oscillator, lookup_lfo_frequency(lfo->settings->frequency));
}

double lfo_transform(void *user_data, double signal, double delta_time) {
    Lfo *lfo = (Lfo*)user_data;
    double delay = lookup_delay(lfo->settings->delay);
    double v = lookup_lfo_amount(lfo->settings->amount) *
        oscillator_transform(&lfo->oscillator, signal, delta_time);

    if (delay > 0 && lfo->t < delay) {
        v *= lfo->t / delay;
    }

    lfo->t += delta_time;
    if (lfo->t > delay) {
        lfo->t = delay;
    }
    return v;
}
