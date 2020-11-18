#include <SDL2/SDL.h>
#include <math.h>
#include "oscillator.h"

#define SAMPLE_RATE 48000.0

void oscillator_reset(Oscillator *oscillator) {
    SDL_memset(oscillator, 0, sizeof(Oscillator));
}

void oscillator_trigger(Oscillator *oscillator, double frequency) {
    oscillator_reset(oscillator);
    oscillator->frequency = frequency;
}

void oscillator_off(Oscillator *oscillator) {
    oscillator->release_t = oscillator->t;
}

double _oscillator_get_amplitude(Oscillator *oscillator) {
    if (oscillator->release_t > 0) {
        double t = oscillator->t - oscillator->release_t;
        double amp = 1/(20*t+1) - 0.03;
        if (amp < 0) {
            oscillator->frequency = 0;
            amp = 0;
        }
        return amp;
    } else {
        return 1;
    }
}

double oscillator_poll(Oscillator *oscillator) {
    if (oscillator->frequency <= 0) {
        return 0;
    }
    double amp = 0;
    for (int i = 1; i < 16; i+=2) {
        double position = i*oscillator->frequency * oscillator->sample * 2.0 * M_PI / SAMPLE_RATE ;
        amp += sin(position)/(double)i;
    }
    if (fabs(amp) > 1.0) {
        printf("Overflow %f", amp);
    }
    double gain = _oscillator_get_amplitude(oscillator);
    if (fabs(gain) > 1.0) {
        printf("Overflow %f", amp);
    }
    amp *= gain;
    oscillator->sample++;
    oscillator->t = oscillator->sample / SAMPLE_RATE;
    return amp;
}
