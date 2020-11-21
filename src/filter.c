#include <stdlib.h>

#include "filter.h"

Filter *filter_create(double f, double q) {
    Filter *filter = calloc(1,sizeof(Filter));
    filter_reset(filter, f,q);
    return filter;
}

void filter_set(Filter *filter, double f, double q) {
    if (f > 0.99) {
        f = 0.99;
    } else if (f < 0.00) {
        f = 0.00;
    }
    filter->f = f;
    filter->q = q;
}

void filter_reset(Filter *filter, double f, double q) {
    filter->stage1 = 0;
    filter->stage2 = 0;
    filter_set(filter, f, q);
}

void filter_trigger(void *user_data, double frequency) {
    Filter *filter = (Filter*)user_data;
    filter->t = 0;
}


double filter_transform(void *user_data, double value, double delta_time) {
    Filter *filter = (Filter*)user_data;

    double f = filter->f - 3.0 * filter->t;
    if (f < 0) {
        f = 0;
    }
    double fb = filter->q + filter->q/(1.0 - f);

    filter->stage1 = filter->stage1 + f *
        (value - filter->stage1 + fb * (filter->stage1 - filter->stage2));
    filter->stage2 = filter->stage2 + f *
        (filter->stage1 - filter->stage2);
    filter->t += delta_time;
    return filter->stage2;
}
