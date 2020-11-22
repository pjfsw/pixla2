#ifndef SRC_LFO_H_
#define SRC_LFO_H_

#include "oscillator.h"

typedef struct {
    double delay;
    double frequency;
    double amount;
    Oscillator oscillator;
    double t;
} Lfo;

void lfo_trigger(void *user_data, double frequency) ;

double lfo_transform(void *user_data, double signal, double delta_time);



#endif /* SRC_LFO_H_ */
