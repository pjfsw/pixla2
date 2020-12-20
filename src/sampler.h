#ifndef SRC_SAMPLER_H_
#define SRC_SAMPLER_H_

#include <stdbool.h>

#include "audio_library.h"

#define NUMBER_OF_SAMPLE_VOICES 8

typedef struct {
    double t;
    bool on;
    Sint8 note;
} SamplerVoice;

typedef struct {
    AudioLibrary *audio_library;
    int next_voice;
    SamplerVoice voices[NUMBER_OF_SAMPLE_VOICES];
} Sampler;

Sampler *sampler_create(AudioLibrary *audio_library);

void sampler_destroy(Sampler *sampler);

void sampler_note_on(Sampler *sampler, int note);

void sampler_note_off(Sampler *sampler, int note);

void sampler_off(Sampler *sampler);

double sampler_poll(Sampler *sampler, double delta_time);


#endif /* SRC_SAMPLER_H_ */
