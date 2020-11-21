#include <SDL2/SDL.h>
#include <math.h>
#include "mixer.h"
#include "synth.h"

#define MIXER_CLIPPING 0.7 // -3 dBFS

void mixer_process_buffer(void *user_data, Uint8 *stream, int len) {
    // valueDBFS = 20*log10(abs(value))
    Mixer *mixer = (Mixer*)user_data;
    float *buffer = (float*)stream;
    for (int t = 0; t < len/4; t+=2) {
        float sample = synth_poll(mixer->synth, 1/mixer->sample_rate);
        float adjusted_sample = mixer->master_volume * sample;
        if (fabs(adjusted_sample) > MIXER_CLIPPING) {
            printf("Overflow %0.2f\n", adjusted_sample);
        }
        if (adjusted_sample > MIXER_CLIPPING) {
            adjusted_sample = MIXER_CLIPPING;
        } else if( adjusted_sample < -MIXER_CLIPPING) {
            adjusted_sample = -MIXER_CLIPPING;
        }
        buffer[t] = adjusted_sample;
        buffer[t+1] = adjusted_sample;
        mixer->t++;
    }
}

void mixer_start(Mixer *mixer) {
    SDL_PauseAudioDevice(mixer->device, 0);
}

void mixer_stop(Mixer *mixer) {
    SDL_PauseAudioDevice(mixer->device, 1);
}

Mixer *mixer_create(Synth *synth) {
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    Mixer *mixer = calloc(1, sizeof(Mixer));
    mixer->synth = synth;
    mixer->master_volume = 0.5;

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
