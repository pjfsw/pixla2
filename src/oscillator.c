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
}

double _oscillator_get_amplitude(Oscillator *oscillator) {
    if (oscillator->t > 1.0) {
        oscillator->frequency = 0;
        return 0;
    } else {
        return 1.0-oscillator->t;
    }
}

double oscillator_poll(Oscillator *oscillator) {
    if (oscillator->frequency <= 0) {
        return 0;
    }
    double amp = sin(oscillator->frequency * oscillator->sample * 2.0 * M_PI / SAMPLE_RATE );
    amp *= _oscillator_get_amplitude(oscillator);
    oscillator->sample++;
    oscillator->t = oscillator->sample / SAMPLE_RATE;
    return amp;
}
