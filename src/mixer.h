#ifndef _MIXER_H_
#define _MIXER_H_

#include <SDL2/SDL.h>
#include "synth.h"

#define MIXER_DEFAULT_SAMPLE_RATE 48000
#define MIXER_DEFAULT_BUFFER_SIZE 512

typedef struct {
    SDL_AudioDeviceID device;
    double sample_rate;
    double master_volume;
    Synth *synth;
    Uint32 t;
} Mixer;

Mixer *mixer_create(Synth *synth);

void mixer_destroy(Mixer *mixer);

void mixer_start(Mixer *mixer);

void mixer_stop(Mixer *mixer);

#endif // _MIXER_H_
