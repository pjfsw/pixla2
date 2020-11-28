#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "filter.h"
#include "vca.h"

#define FILTER_MAX 0.999
#define FILTER_MIN 0.001

#define Q_MAX 0.999

void filter_set(FilterSettings *settings, double f, double q) {
    settings->f = fmin(1, fmax(f, 0));
    settings->q = fmin(0.9, fmax(q, 0));
}

void filter_trigger(void *user_data, double frequency) {
    Filter *filter = (Filter*)user_data;
    filter->t = 0;
    vca_trigger(&filter->vca, frequency);
}

void filter_off(void *user_data) {
    Filter *filter = (Filter*)user_data;
    vca_off(&filter->vca);
}


double filter_transform(void *user_data, double value, double delta_time) {
    Filter *filter = (Filter*)user_data;
    FilterSettings *settings = (FilterSettings*)filter->settings;
    //return value;
    double f = vca_transform(&filter->vca, settings->f, delta_time);

    f = fmin(FILTER_MAX, fmax(FILTER_MIN, f));

    double fb = settings->q + settings->q/(1.0 - f);

    filter->stage1 = filter->stage1 + f *
        (value - filter->stage1 + fb * (filter->stage1 - filter->stage2));
    filter->stage2 = filter->stage2 + f *
        (filter->stage1 - filter->stage2);
    filter->t += delta_time;
    return filter->stage2;
}
