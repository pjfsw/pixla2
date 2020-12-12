#ifndef SRC_SAMPLER_H_
#define SRC_SAMPLER_H_

#include <stdbool.h>

#include "audio_library.h"

typedef struct {
    AudioLibrary *audio_library;
    double t;
    bool on;
    Sint8 note;
} Sampler;

Sampler *sampler_create(AudioLibrary *audio_library);

void sampler_destroy(Sampler *sampler);

void sampler_note_on(Sampler *sampler, int note);

void sampler_note_off(Sampler *sampler, int note);

void sampler_off(Sampler *sampler);

double sampler_poll(Sampler *sampler, double delta_time);


#endif /* SRC_SAMPLER_H_ */
