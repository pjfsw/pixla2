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

void oscillator_set_fm(Oscillator *oscillator, double fm) {
    oscillator->fm = fm;
}

void oscillator_trigger(void *user_data, double frequency) {
    Oscillator *oscillator = (Oscillator *)user_data;
    oscillator->frequency = frequency;
}

double _oscillator_generate(Oscillator *oscillator,  double delta_time) {
    if (oscillator == NULL) {
        fprintf(stderr, "oscillator is null\n");
        return 0.0;
    }
    if (oscillator->settings == NULL) {
        fprintf(stderr, "oscillator %d settings is null\n", oscillator->tag);
        return 0.0;
    }
    double frequency = oscillator->frequency * pow(2, oscillator->fm);
    switch(oscillator->settings->transpose) {
    case TRANSP_OCT_DOWN:
        frequency *= 0.5;
        break;
    case TRANSP_2OCT_DOWN:
        frequency *= 0.25;
        break;
    case TRANSP_3OCT_DOWN:
        frequency *= 0.125;
        break;
    case TRANSP_OCT_UP:
        frequency *= 2;
        break;
    case TRANSP_2OCT_UP:
        frequency *= 4;
        break;
    case TRANSP_3OCT_UP:
        frequency *= 8;
        break;
    default:
        break;
    }
    if (frequency <= 0) {
        return 0;
    }
    double cycle_time = 1/frequency;
    double amp = 0;
    if (oscillator->settings->waveform == SQUARE) {
        amp = oscillator->t > 0.5 * cycle_time ? 1 : -1;
    } else if (oscillator->settings->waveform == SAW) {
        double t = oscillator->t * frequency;
        amp = 2*(t - floor(t+0.5));
    } else if (oscillator->settings->waveform == TRIANGLE) {
        // 0 : 0.25 =  0 : 1
        // 0.25 : 0.5 = 1 : 0
        // 0.5 : 0.75 = 0 : -1
        // 0.75 : 1 = -1 : 0
        double t = oscillator->t * frequency;
        t  = t - floor(t);
        if (t < 0.25) {
            amp = 4 * t;
        } else if (t < 0.75) {
            amp = 2 - 4 * t;
        } else {
            amp = 4 * t - 4;
        }
    } else if (oscillator->settings->waveform == SINE) {
        amp = sin(frequency * oscillator->t * 2.0 * M_PI);
    } else if (oscillator->settings->waveform == NOISE) {
        amp = (double)rand()/(double)RAND_MAX;
    }

    if (fabs(amp) > 1.0) {
        fprintf(stderr, "Oscillator overflow %f\n", amp);
    }
    oscillator->t += delta_time;
    if (oscillator->t > cycle_time) {
        oscillator->t -= cycle_time;
    }

    return amp;
}

double oscillator_transform(void *user_data, double value, double delta_time) {
    return _oscillator_generate((Oscillator *)user_data, delta_time);
}
