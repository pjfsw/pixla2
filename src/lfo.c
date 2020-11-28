#include "lfo.h"
#include "oscillator.h"

double _lfo_normalize_frequency(double frequency) {
    return frequency * 10;
}

double _lfo_normalize_delay(double delay) {
    return delay * 20;
}

double _lfo_normalize_amount(double amount) {
    return amount * 0.25;
}

void lfo_trigger(void *user_data, double frequency) {
    Lfo *lfo = (Lfo*)user_data;
    lfo->t = 0;
    oscillator_trigger(&lfo->oscillator, _lfo_normalize_frequency(lfo->settings->frequency));
}

double lfo_transform(void *user_data, double signal, double delta_time) {
    Lfo *lfo = (Lfo*)user_data;
    double delay = _lfo_normalize_delay(lfo->settings->delay);
    double v = _lfo_normalize_amount(lfo->settings->amount) * oscillator_transform(&lfo->oscillator, signal, delta_time);
    if (delay > 0 && lfo->t < delay) {
        v *= lfo->t / delay;
    }

    lfo->t += delta_time;
    if (lfo->t > delay) {
        lfo->t = delay;
    }
    return v;
}
