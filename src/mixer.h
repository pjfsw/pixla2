#ifndef _MIXER_H_
#define _MIXER_H_

#include <SDL2/SDL.h>
#include "instrument.h"

#define MIXER_DEFAULT_SAMPLE_RATE 48000
#define MIXER_DEFAULT_BUFFER_SIZE 1024
// dBFS = 20 * log(abs(voltage))
// voltage =
#define MIXER_CLIPPING 0.94 // -0.5 dBFS
#define MIXER_THRESHOLD 0.707 // -3 dBFS

#define LR_DELAY 24
#define LOUDNESS_BUFFER 128

typedef struct {
    Uint8 master_volume;
    Uint8 instr_volume[8];
} MixerSettings;

typedef struct {
    MixerSettings settings;
    SDL_AudioDeviceID device;
    double sample_rate;
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
} Mixer;

Mixer *mixer_create(Instrument *instruments, int number_of_instruments);

void mixer_destroy(Mixer *mixer);

void mixer_start(Mixer *mixer);

void mixer_stop(Mixer *mixer);

#endif // _MIXER_H_
