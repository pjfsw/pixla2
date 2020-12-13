#include <stdlib.h>

#include "sampler.h"

Sampler *sampler_create(AudioLibrary *audio_library) {
    Sampler *sampler = calloc(1, sizeof(Sampler));
    sampler->audio_library = audio_library;
    return sampler;
}

void sampler_destroy(Sampler *sampler) {
    if (sampler == NULL) {
        return;
    }
    free(sampler);
}

void sampler_note_on(Sampler *sampler, int note) {
    sampler->t = 0;
    sampler->on = true;
    sampler->note = note;
}

void sampler_note_off(Sampler *sampler, int note) {
    sampler->on = false;
}

void sampler_off(Sampler *sampler) {
    sampler->on = false;
}

double sampler_poll(Sampler *sampler, double delta_time) {
    if (!sampler->on) {
        return 0;
    }
    double sample = audio_library_get(sampler->audio_library, sampler->note % 12, sampler->t);
    if (sample < -1 || sample > 1) {
        fprintf(stderr, "Sample overload %f\n", sample);
        sample = 0;
    }
    sampler->t+=delta_time;

    return sample;
}
