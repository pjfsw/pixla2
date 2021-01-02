#ifndef SRC_FILTER_H_
#define SRC_FILTER_H_

#include "vca.h"

typedef struct {
    Uint8 f;
    Uint8 q;
} FilterSettings;

typedef struct {
    FilterSettings *settings;
    Vca vca;
    double t;
    double stage1;
    double stage2;
    double filter_mod;
} Filter;

Filter *filter_create(double f, double q);

void filter_trigger(void *user_data, double frequency, Uint8 velocity);

void filter_mod(Filter *filter, double filter_mod);

void filter_off(void *user_data);

double filter_transform(void *user_data, double value, double delta_time);

#endif /* SRC_FILTER_H_ */
