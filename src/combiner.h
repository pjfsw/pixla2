#ifndef SRC_COMBINER_H_
#define SRC_COMBINER_H_

#include "oscillator.h"
#include "vca.h"
#include "lfo.h"

typedef enum {
    COMB_ADD,
    COMB_MULTIPLY,
    COMB_MODULATE
} CombineMode;

typedef enum {
    STRENGTH_MANUAL,
    STRENGTH_VCA
} CombineStrengthMode;

typedef struct {
    CombineMode combine_mode;
    CombineStrengthMode strength_mode;
    double oscillator2_strength;
    double oscillator2_scale;
    double detune;
} CombinerSettings;

typedef struct {
    CombinerSettings *settings;
    Oscillator *oscillator1;
    Oscillator *oscillator2;
    Vca vca;
    double frequency;
} Combiner;

void combiner_set_fm(Combiner *combiner, double fm);

void combiner_trigger(void *user_data, double frequency);

double combiner_transform(void *user_data, double value, double delta_time);

void combiner_off(void *user_data);


#endif /* SRC_COMBINER_H_ */
