#ifndef SRC_COMBINER_H_
#define SRC_COMBINER_H_

#include "oscillator.h"

typedef struct {
    Oscillator *oscillator1;
    Oscillator *oscillator2;
    double frequency;
} Combiner;

void combiner_trigger(void *user_data, double frequency);

double combiner_transform(void *user_data, double value, double delta_time);



#endif /* SRC_COMBINER_H_ */
