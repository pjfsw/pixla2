#ifndef _MIXER_H_
#define _MIXER_H_

#include <SDL2/SDL.h>
#include "instrument.h"

#define MIXER_DEFAULT_SAMPLE_RATE 48000
#define MIXER_DEFAULT_BUFFER_SIZE 256

#define LR_DELAY 24

typedef struct {
    SDL_AudioDeviceID device;
    double divisor;
    double sample_rate;
    double master_volume;
    Instrument *instruments;
    int number_of_instruments;
    int tap_size;
    float *left_tap;
    float *right_tap;
    int delay_pos;
    float lr_delay[LR_DELAY];
} Mixer;

Mixer *mixer_create(Instrument *instruments, int number_of_instruments, double divisor);

void mixer_destroy(Mixer *mixer);

void mixer_start(Mixer *mixer);

void mixer_stop(Mixer *mixer);

#endif // _MIXER_H_
