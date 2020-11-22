#include <SDL2/SDL.h>
#include <math.h>
#include "oscillator.h"

#define SAMPLE_RATE 48000.0

#define OSCILLATOR_P0 0.3190
#define OSCILLATOR_P1 0.7756
#define OSCILLATOR_P2 0.9613
#define OSCILLATOR_A0 0.02109238
#define OSCILLATOR_A1 0.07113478
#define OSCILLATOR_A2 0.68873558
#define OSCILLATOR_OFFSET (OSCILLATOR_A0 + OSCILLATOR_A1 + OSCILLATOR_A2)

void oscillator_reset(Oscillator *oscillator) {
    SDL_memset(oscillator, 0, sizeof(Oscillator));
}

void oscillator_set_waveform(Oscillator *oscillator, Waveform waveform) {
    oscillator->waveform = waveform;
}


void oscillator_trigger(void *user_data, double frequency) {
    Oscillator *oscillator = (Oscillator *)user_data;
    oscillator->frequency = frequency;
    if (oscillator->frequency > 0) {
        oscillator->cycle_time = 1/frequency;
    }
}

void _oscillator_add_time(Oscillator *oscillator, double delta_time) {
    oscillator->t += delta_time;
    if (oscillator->t > oscillator->cycle_time) {
        oscillator->t -= oscillator->cycle_time;
    }
}

double _oscillator_generate(Oscillator *oscillator,  double delta_time) {
    if (oscillator->frequency <= 0) {
        return 0;
    }
    double amp = 0;
    /*for (int i = 1; i < 256; i+=2) {
        double position = i * oscillator->frequency * oscillator->t * 2.0 * M_PI;
        amp += sin(position)/i;
    }*/
    if (oscillator->waveform == SQUARE) {
        amp = oscillator->t > 0.5 * oscillator->cycle_time ? 1 : -1;
    } else if (oscillator->waveform == SAW) {
        double t = oscillator->t * oscillator->frequency;
        amp = 2*(t - floor(t+0.5));
    } else if (oscillator->waveform == SINE) {
        amp = sin(oscillator->frequency * oscillator->t * 2.0 * M_PI);
    } else if (oscillator->waveform == NOISE) {
        float temp = (float)rand();
        oscillator->state[0] = OSCILLATOR_P0 * (oscillator->state[0] - temp) + temp;
        temp = (float)rand();
        oscillator->state[1] = OSCILLATOR_P1 * (oscillator->state[1] - temp) + temp;
        temp = (float)rand();
        oscillator->state[2] = OSCILLATOR_P2 * (oscillator->state[2] - temp) + temp;
        amp = (
            OSCILLATOR_A0 * oscillator->state[0] +
            OSCILLATOR_A1 * oscillator->state[1] +
            OSCILLATOR_A2 * oscillator->state[2]
                                              ) * 2.0 / (float)RAND_MAX - OSCILLATOR_OFFSET;
        //amp = (double)rand()/(double)RAND_MAX;

    }

    if (fabs(amp) > 1.0) {
        printf("Overflow %f", amp);
    }
    _oscillator_add_time(oscillator, delta_time);
    return amp;
}

double oscillator_transform(void *user_data, double value, double delta_time) {
    return _oscillator_generate((Oscillator *)user_data, delta_time);
}
