#include <SDL2/SDL.h>
#include <math.h>
#include "oscillator.h"

#define SAMPLE_RATE 48000.0

void oscillator_reset(Oscillator *oscillator) {
    SDL_memset(oscillator, 0, sizeof(Oscillator));
}

void oscillator_trigger(void *user_data, double frequency) {
    Oscillator *oscillator = (Oscillator *)user_data;
    oscillator_reset(oscillator);
    oscillator->frequency = frequency;
    if (oscillator->frequency > 0) {
        oscillator->cycle_time = 1/frequency;
    }
}

double _oscillator_generate(Oscillator *oscillator,  double delta_time) {
    if (oscillator->frequency <= 0) {
        return 0;
    }
    double amp = 0;
    /*for (int i = 1; i < 256; i+=2) {
        double position = i * oscillator->frequency * oscillator->t * 2.0 * M_PI;
        amp += oscillator_divisor_table[i] * sin(position);
    }*/
    amp = oscillator->t > 0.5 * oscillator->cycle_time ? 1 : -1;

    if (fabs(amp) > 1.0) {
        printf("Overflow %f", amp);
    }
    oscillator->t += delta_time;
    if (oscillator->t > oscillator->cycle_time) {
        oscillator->t -= oscillator->cycle_time;
    }
    return amp;
}

double oscillator_transform(void *user_data, double value, double delta_time) {
    return _oscillator_generate((Oscillator *)user_data, delta_time);
}
