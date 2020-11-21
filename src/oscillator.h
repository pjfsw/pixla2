#ifndef SRC_OSCILLATOR_H_
#define SRC_OSCILLATOR_H_

#include <stdbool.h>

typedef enum {
    SQUARE=0,
    SAW=1
} Waveform;

typedef struct {
    double frequency;
    double cycle_time;
    double t;
    Waveform waveform;
} Oscillator;

void oscillator_reset(Oscillator *oscillator);

void oscillator_set_waveform(Oscillator *oscillator, Waveform waveform);

void oscillator_trigger(void *user_data, double frequency);

double oscillator_transform(void *user_data, double value, double delta_time);

#endif /* SRC_OSCILLATOR_H_ */
