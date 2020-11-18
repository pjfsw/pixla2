#include "mixer.h"
#include <SDL2/SDL.h>
#include <math.h>

float _mixer_get_next_sample(Mixer *mixer) {
    float frequency = 440;
    return sin(frequency * (double)mixer->t * 2.0 * M_PI / mixer->sample_rate);
}

void mixer_process_buffer(void *user_data, Uint8 *stream, int len) {
    Mixer *mixer = (Mixer*)user_data;
    float *buffer = (float*)stream;
    for (int t = 0; t < len/4; t+=2) {
        float sample = _mixer_get_next_sample(mixer);
        buffer[t] = 0.7*sample;
        buffer[t+1] = 0.7*sample;
        mixer->t++;
    }
}

void mixer_start(Mixer *mixer) {
    SDL_PauseAudioDevice(mixer->device, 0);
}

void mixer_stop(Mixer *mixer) {
    SDL_PauseAudioDevice(mixer->device, 1);
}

Mixer *mixer_create() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    Mixer *mixer = calloc(1, sizeof(Mixer));

    SDL_AudioSpec want;
    SDL_AudioSpec have;

    SDL_memset(&want, 0, sizeof(want));
    want.freq = MIXER_DEFAULT_SAMPLE_RATE;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = MIXER_DEFAULT_BUFFER_SIZE;
    want.callback = mixer_process_buffer;
    want.userdata = mixer;

    mixer->device = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    if (!mixer->device) {
        SDL_Log("Failed to open audio: %s", SDL_GetError());
        mixer_destroy(mixer);
        return NULL;
    }
    mixer->sample_rate = have.freq;
    printf("Sample rate %f Hz\n", mixer->sample_rate);
    return mixer;
}

void mixer_destroy(Mixer *mixer) {
    if (mixer != NULL) {
        SDL_CloseAudioDevice(mixer->device);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        free(mixer);
    }
}
