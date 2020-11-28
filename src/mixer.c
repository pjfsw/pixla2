#include <SDL2/SDL.h>
#include <math.h>
#include "mixer.h"
#include "synth.h"
#include "vca.h"
#include "midi_notes.h"

// dBFS = 20 * log(abs(voltage))
// voltage =
#define MIXER_CLIPPING 0.75 // -2.5 dBFS

void mixer_process_buffer(void *user_data, Uint8 *stream, int len) {
    // valueDBFS = 20*log10(abs(value))
    Mixer *mixer = (Mixer*)user_data;
    float *buffer = (float*)stream;
    int tapCount = 0;
    double delta_time = 1/mixer->sample_rate;
    for (int t = 0; t < len/4; t+=2) {
        double sample = 0.0;
        for (int n = 0; n < mixer->number_of_synths; n++) {
            sample += mixer->synths[n]->master_level * synth_poll(mixer->synths[n], delta_time);
        }
        float adjusted_sample = mixer->master_volume * sample / mixer->number_of_synths;

        if (fabs(adjusted_sample) > MIXER_CLIPPING) {
            printf("Overflow %0.2f\n", adjusted_sample);
        }
        if (adjusted_sample > MIXER_CLIPPING) {
            adjusted_sample = MIXER_CLIPPING;
        } else if( adjusted_sample < -MIXER_CLIPPING) {
            adjusted_sample = -MIXER_CLIPPING;
        }
        buffer[t] = adjusted_sample;
        mixer->lr_delay[mixer->delay_pos] = adjusted_sample;
        mixer->delay_pos = (mixer->delay_pos + 1) % LR_DELAY;
        buffer[t+1] = mixer->lr_delay[mixer->delay_pos];

        mixer->left_tap[tapCount] = buffer[t];
        mixer->right_tap[tapCount] = buffer[t+1];

        tapCount++;
    }
}

void mixer_start(Mixer *mixer) {
    SDL_PauseAudioDevice(mixer->device, 0);
}

void mixer_stop(Mixer *mixer) {
    SDL_PauseAudioDevice(mixer->device, 1);
}

Mixer *mixer_create(Synth **synths, int number_of_synths) {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    midi_notes_init();

    Mixer *mixer = calloc(1, sizeof(Mixer));
    mixer->synths = calloc(number_of_synths, sizeof(Synth*));
    for (int i = 0; i < number_of_synths; i++) {
        mixer->synths[i] = synths[i];
    }
    mixer->number_of_synths = number_of_synths;
    mixer->master_volume = 0.7;
    mixer->tap_size = MIXER_DEFAULT_BUFFER_SIZE;
    mixer->left_tap = calloc(mixer->tap_size, sizeof(float));
    mixer->right_tap = calloc(mixer->tap_size, sizeof(float));

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
        free(mixer->left_tap);
        free(mixer->right_tap);
        free(mixer->synths);
        free(mixer);
    }
}
