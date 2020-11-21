#ifndef SRC_OSCILLATOR_H_
#define SRC_OSCILLATOR_H_

#include <stdbool.h>

typedef struct {
    double frequency;
    double cycle_time;
    double t;
} Oscillator;

void oscillator_reset(Oscillator *oscillator);

void oscillator_trigger(void *user_data, double frequency);

double oscillator_transform(void *user_data, double value, double delta_time);

#endif /* SRC_OSCILLATOR_H_ */
