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
    filter->fb = q + q/(1.0 - f);
}

void filter_reset(Filter *filter, double f, double q) {
    filter->stage1 = 0;
    filter->stage2 = 0;
    filter_set(filter, f, q);
}

double filter_transform(void *user_data, double value, double delta_time) {
    Filter *filter = (Filter*)user_data;

    filter->stage1 = filter->stage1 + filter->f *
        (value - filter->stage1 + filter->fb * (filter->stage1 - filter->stage2));
    filter->stage2 = filter->stage2 + filter->f *
        (filter->stage1 - filter->stage2);
    return filter->stage2;
}
