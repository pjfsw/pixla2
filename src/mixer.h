#ifndef _MIXER_H_
#define _MIXER_H_

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "instrument.h"

#define MIXER_DEFAULT_SAMPLE_RATE 48000
#define MIXER_DEFAULT_BUFFER_SIZE 512
// dBFS = 20 * log(abs(voltage))
// voltage =
#define MIXER_CLIPPING 0.94 // -0.5 dBFS
#define MIXER_THRESHOLD 0.707 // -3 dBFS

#define LR_DELAY 24
#define LOUDNESS_BUFFER 128

typedef int(*MixerTriggerFunc)(void *);

typedef struct {
    Uint8 master_volume;
    Uint8 instr_volume[8];
} MixerSettings;

typedef struct _Mixer {
    MixerSettings settings;
    SDL_AudioDeviceID device;
    double sample_rate;
    MixerTriggerFunc mixer_trigger_func;
    void *mixer_trigger_func_user_data;
    Instrument *instruments;
    int number_of_instruments;
    int tap_size;
    float *left_tap;
    float *right_tap;
    int delay_pos;
    float lr_delay[LR_DELAY];
    float loudness_buffer[LOUDNESS_BUFFER];
    float loudness_sum;
    float loudness;
    int loudness_pos;
    int player_delay;
} Mixer;

Mixer *mixer_create(Instrument *instruments,  int number_of_instruments,
    MixerTriggerFunc mixer_trigger_func, void *mixer_trigger_func_user_data, bool attach_audio_device);

void mixer_process_buffer(Mixer *mixer, float *buffer, int number_of_floats);

void mixer_destroy(Mixer *mixer);

void mixer_start(Mixer *mixer);

void mixer_stop(Mixer *mixer);

#endif // _MIXER_H_
