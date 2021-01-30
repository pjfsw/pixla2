#ifndef SRC_AUDIO_LIBRARY_H_
#define SRC_AUDIO_LIBRARY_H_

#include <SDL2/SDL.h>

#define MAX_AUDIO_DATA 12

typedef struct {
    SDL_AudioSpec spec;
    Uint8 *data;
    Uint8 sample_step;
    Uint32 length_bytes;
    Uint32 length_samples;
} AudioData;

typedef struct {
    AudioData audio[MAX_AUDIO_DATA];
} AudioLibrary;

AudioLibrary *audio_library_create(char *path);

double audio_library_get(AudioLibrary *audio_library, int i, double t);

void audio_library_destroy(AudioLibrary *audio_library);

#endif /* SRC_AUDIO_LIBRARY_H_ */
