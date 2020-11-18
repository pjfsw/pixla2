#ifndef SRC_OSCILLATOR_H_
#define SRC_OSCILLATOR_H_

#include <stdbool.h>

typedef struct {
    double frequency;
    int sample;
    double t;
    double release_t;
    int wave_sample;
} Oscillator;

void oscillator_reset(Oscillator *oscillator);

void oscillator_trigger(Oscillator *oscillator, double frequency);

void oscillator_off(Oscillator *oscillator);

double oscillator_poll(Oscillator *oscillator);

#endif /* SRC_OSCILLATOR_H_ */
