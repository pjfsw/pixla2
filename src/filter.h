#ifndef SRC_FILTER_H_
#define SRC_FILTER_H_

#include "vca.h"

typedef struct {
    Vca vca;
    double t;
    double f;
    double q;
    double stage1;
    double stage2;
} Filter;

Filter *filter_create(double f, double q);

void filter_set(Filter *filter, double f, double q);

void filter_reset(Filter *filter, double f, double q);

void filter_trigger(void *user_data, double frequency);

void filter_off(void *user_data);

double filter_transform(void *user_data, double value, double delta_time);

#endif /* SRC_FILTER_H_ */
