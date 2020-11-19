#ifndef SRC_OSCILLATOR_H_
#define SRC_OSCILLATOR_H_

#include <stdbool.h>

typedef struct {
    double frequency;
    double t;
} Oscillator;

void oscillator_reset(Oscillator *oscillator);

void oscillator_trigger(Oscillator *oscillator, double frequency);

double oscillator_poll(Oscillator *oscillator, double delta_time);

#endif /* SRC_OSCILLATOR_H_ */
