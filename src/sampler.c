#include <stdlib.h>

#include "sampler.h"
#include "lookup_tables.h"

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

int sampler_note_on(Sampler *sampler, int note, int velocity) {
    for (int i = 0; i < NUMBER_OF_SAMPLE_VOICES; i++) {
        SamplerVoice *voice = &sampler->voices[i];
        if (!voice->on) {
            voice->t = 0;
            voice->on = true;
            voice->note = note;
            voice->level = lookup_volume(velocity);
            return i;
        }
    }
    printf("Failed to find sampler voice\n");
    return 0;
}

void sampler_note_off(Sampler *sampler, int voice_id) {
    if (voice_id < 0 || voice_id >= NUMBER_OF_SAMPLE_VOICES) {
        return;
    }
    SamplerVoice *voice = &sampler->voices[voice_id];
    voice->on = false;
}

void sampler_off(Sampler *sampler) {
    for (int i = 0; i < NUMBER_OF_SAMPLE_VOICES; i++) {
        SamplerVoice *voice = &sampler->voices[i];
        voice->on = false;
    }
}

double sampler_poll(Sampler *sampler, double delta_time) {
    double sample = 0;
    for (int i = 0; i < NUMBER_OF_SAMPLE_VOICES; i++) {
        SamplerVoice *voice = &sampler->voices[i];
        if (!voice->on) {
            continue;
        }
        double this_sample = audio_library_get(sampler->audio_library, voice->note % 12, voice->t);
        if (this_sample < -1 || this_sample > 1) {
            fprintf(stderr, "Sample overload %f\n", this_sample);
            this_sample = 0;
        }
        sample += voice->level*this_sample;
        voice->t+=delta_time;
    }

    return sample;
}
