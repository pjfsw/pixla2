#include "lfo.h"
#include "oscillator.h"

void lfo_trigger(void *user_data, double frequency) {
    Lfo *lfo = (Lfo*)user_data;
    lfo->t = 0;
    oscillator_trigger(&lfo->oscillator, lfo->settings->frequency);
}

double lfo_transform(void *user_data, double signal, double delta_time) {
    Lfo *lfo = (Lfo*)user_data;
    double v = lfo->settings->amount * oscillator_transform(&lfo->oscillator, signal, delta_time);
    if (lfo->settings->delay > 0 && lfo->t < lfo->settings->delay) {
        v *= lfo->t / lfo->settings->delay;
    }

    lfo->t += delta_time;
    if (lfo->t > lfo->settings->delay) {
        lfo->t = lfo->settings->delay;
    }
    return v;
}
