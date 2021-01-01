#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "filter.h"
#include "vca.h"
#include "lookup_tables.h"


void filter_trigger(void *user_data, double frequency) {
    Filter *filter = (Filter*)user_data;
    filter->t = 0;
    vca_trigger(&filter->vca, frequency);
}

void filter_off(void *user_data) {
    Filter *filter = (Filter*)user_data;
    vca_off(&filter->vca);
}

void filter_mod(Filter *filter, double filter_mod) {
    filter->filter_mod = filter_mod;
}

double filter_transform(void *user_data, double value, double delta_time) {
    Filter *filter = (Filter*)user_data;
    FilterSettings *settings = (FilterSettings*)filter->settings;
    //return value;
    double f = lookup_filter_frequency(settings->f);
    f += 0.5 * filter->filter_mod;
    f = vca_transform(&filter->vca, f, delta_time);
    if (f < 0.001) {
        f = 0.001;
    }
    if (f > 0.999) {
        f = 0.999;
    }
    double q = lookup_filter_q(settings->q);

    double fb = q + q/(1.0 - f);

    filter->stage1 = filter->stage1 + f *
        (value - filter->stage1 + fb * (filter->stage1 - filter->stage2));
    filter->stage2 = filter->stage2 + f *
        (filter->stage1 - filter->stage2);
    filter->t += delta_time;
    return filter->stage2;
}
