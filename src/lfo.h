#ifndef SRC_LFO_H_
#define SRC_LFO_H_

#include <SDL2/SDL.h>
#include "oscillator.h"

typedef struct {
    Uint8 delay;
    Uint8 frequency;
    Uint8 amount;
    OscillatorSettings oscillator;
} LfoSettings;

typedef struct {
    LfoSettings *settings;
    Oscillator oscillator;
    double t;
} Lfo;

void lfo_trigger(void *user_data, double frequency) ;

double lfo_transform(void *user_data, double signal, double delta_time);



#endif /* SRC_LFO_H_ */
