#ifndef SRC_COMBINER_H_
#define SRC_COMBINER_H_

#include "oscillator.h"

typedef enum {
    COMB_ADD,
    COMB_SUBTRACT,
    COMB_MULTIPLY,
    COMB_MODULATE
} CombineMode;

typedef struct {
    Oscillator *oscillator1;
    Oscillator *oscillator2;
    double frequency;
    CombineMode combine_mode;
    double oscillator2_strength;
} Combiner;

void combiner_set_mode(Combiner *combiner, CombineMode mode);

void combiner_set_oscillator2_strength(Combiner *combiner, double strength);

void combiner_trigger(void *user_data, double frequency);

double combiner_transform(void *user_data, double value, double delta_time);



#endif /* SRC_COMBINER_H_ */
