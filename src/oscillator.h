#ifndef SRC_OSCILLATOR_H_
#define SRC_OSCILLATOR_H_

#include <stdbool.h>

typedef enum {
    SQUARE=0,
    SAW=1,
    SINE=2,
    NOISE=3
} Waveform;

typedef struct {
    double frequency;
    double cycle_time;
    double phase;
    double t;
    double state[3];
    Waveform waveform;
} Oscillator;

void oscillator_reset(Oscillator *oscillator);

void oscillator_set_waveform(Oscillator *oscillator, Waveform waveform);

void oscillator_trigger(void *user_data, double frequency);

void oscillator_set_phase(Oscillator *oscillator, double phase);

double oscillator_transform(void *user_data, double value, double delta_time);

#endif /* SRC_OSCILLATOR_H_ */
