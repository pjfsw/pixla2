#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <limits.h>

#include "audio_library.h"

bool _audio_library_load_wav(AudioData *audio, char *path) {
    if (NULL == SDL_LoadWAV(
        path,
        &audio->spec,
        &audio->data,
        &audio->length_bytes
        )) {
        fprintf(stderr, "Failed to open file %s: %s\n", path, SDL_GetError());
        return false;
    }
    SDL_AudioSpec *spec = &audio->spec;
    printf("WAV Channels: %d\n", spec->channels);
    printf("WAV Freq: %d\n", spec->freq);
    if (SDL_AUDIO_ISFLOAT(spec->format)) {
        printf("  floating point data\n");
    } else {
        if (SDL_AUDIO_ISSIGNED(spec->format)) {
            printf("  signed\n");
        }
        if (SDL_AUDIO_ISLITTLEENDIAN(spec->format)) {
            printf("  little endian\n");
        }
    }
    printf("%d bits per sample\n", (int) SDL_AUDIO_BITSIZE(spec->format));
    audio->sample_step = SDL_AUDIO_BITSIZE(spec->format) * spec->channels / 8;
    printf("Step %d\n", audio->sample_step);
    audio->length_samples = audio->length_bytes/audio->sample_step;
    printf("WAV Padding: %d\n", spec->padding);
    printf("WAV Samples: %d\n", spec->samples);
    return true;
}

AudioLibrary *audio_library_create(char *path) {
    AudioLibrary *library = calloc(1, sizeof(AudioLibrary));
    if (!_audio_library_load_wav(&library->audio[0], "/Users/johanfr/kick.wav") ||
        !_audio_library_load_wav(&library->audio[1], "/Users/johanfr/snare.wav") ||
        !_audio_library_load_wav(&library->audio[2], "/Users/johanfr/drop3.wav")) {
        audio_library_destroy(library);
        return NULL;
    }

    return library;
}

double audio_library_get(AudioLibrary *audio_library, int i, double t) {
    if (audio_library == NULL || i < 0 || i >= MAX_AUDIO_DATA) {
        return 0;
    }
    AudioData *data = &audio_library->audio[i];
    int sample = (int)(t * (double)data->spec.freq);
    if (sample >= data->length_samples) {
        return 0;
    }
    if (SDL_AUDIO_ISFLOAT(data->spec.format)) {
        float *v = (float*)&data->data[sample*data->sample_step];
        return *v;
    } else if (SDL_AUDIO_ISSIGNED(data->spec.format) && SDL_AUDIO_ISLITTLEENDIAN(data->spec.format)) {
        Sint32 *v = (Sint32*)&data->data[sample*data->sample_step];
        return *v/(double)INT_MAX;
    } else {
        return 0;
    }
}

void audio_library_destroy(AudioLibrary *audio_library) {
    if (audio_library == NULL) {
        return;
    }
    for (int i = 0; i < MAX_AUDIO_DATA; i++) {
        if (audio_library->audio[i].data != NULL) {
            SDL_FreeWAV(audio_library->audio[i].data);
        }
    }
    free(audio_library);
}
