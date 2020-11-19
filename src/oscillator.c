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

double oscillator_poll(Oscillator *oscillator, double delta_time) {
    if (oscillator->frequency <= 0) {
        return 0;
    }
    double amp = 0;
    for (int i = 1; i < 16; i+=2) {
        double position = i * oscillator->frequency * oscillator->t * 2.0 * M_PI;
        amp += sin(position)/(double)i;
    }
    if (fabs(amp) > 1.0) {
        printf("Overflow %f", amp);
    }
    oscillator->t += delta_time;
    return amp;
}
